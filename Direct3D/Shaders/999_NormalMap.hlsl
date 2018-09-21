#include "__CommonHeader.hlsl"
#include "_LightHeader.hlsl"

cbuffer VS_Bone : register(b2)
{
    matrix Bones[128];
}

cbuffer VS_Render : register(b3)
{
    //본 계산을 여기서 한다.
    int BoneNumber;
}

cbuffer VS_Plane : register(b4)
{
    float4 ClipPlane;
}

struct PixelInput
{
    float4 wPosition : POSITION0;
    float4 vPosition : POSITION1;
    float4 pPosition : SV_POSITION;

    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 view : VIEW0;

    float clip : SV_ClipDistance0; //PS 에서 SV_ClipDistance0(clip)값에 따라 처리하지 않음
};

PixelInput VS(VertexTextureNormalTangentBlend input)
{
    PixelInput output;
    
    output.wPosition = GetWorld(input.position, Bones[BoneNumber]);
    output.vPosition = GetView(output.wPosition);
    output.pPosition = GetProjection(output.vPosition);

    output.normal = normalize(mul(input.normal, (float3x3) Bones[BoneNumber]));
    output.tangent = normalize(mul(input.tangent, (float3x3) Bones[BoneNumber]));

    //정점에서 카메라로 나가도록
    output.view = normalize(GetViewPosition() - output.wPosition.xyz);

    output.uv = input.uv;

    output.clip = dot(output.wPosition, ClipPlane);

    return output;
}

void TangentSpace(inout float3 bump, float3 normalMap, float3 normal, float3 tangent)
{
    //0~1 -> -1~1 로 변경
    float3 coord = 2.0f * normalMap - 1.0f; 

    float3 N = normal; //Z
    float3 T = normalize(tangent - dot(tangent, N) * N); //X
    float3 B = cross(N, T); //Y

    float3x3 TBN = float3x3(T, B, N);
    
    bump = mul(coord, TBN);
}


float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    //Diffuse(color, _direction, input.normal);
    //Specular(color, _direction, input.normal, input.view);

    float4 diffuse = _diffuseMap.Sample(_diffuseSampler, input.uv);
    //Diffuse(color, diffuse.rgb, _direction, input.normal);

    float4 normalMap = _normalMap.Sample(_normalSampler, input.uv);
    float3 bump = 0;

    TangentSpace(bump, normalMap.rgb, input.normal, input.tangent);

    float intensity = saturate(dot(bump, -_direction));
    color = diffuse * intensity;

    float4 specular = _specularMap.Sample(_specularSampler, input.uv);
    Specular(color.rgb, specular.rgb, input.normal, input.view);

    PointLightingAll(color.rgb, input.wPosition, input.normal);
    SpotLightingAll(color.rgb, input.wPosition, input.normal);

    return color;
}