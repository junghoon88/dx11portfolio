#include "../__CommonHeader.hlsl"
#include "../_NormalHeader.hlsl"
#include "../_LightHeader.hlsl"

cbuffer VS_Bone : register(b2)
{
	matrix Bones[128];
}

cbuffer VS_Render : register(b3)
{
	int BoneNumber;
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

};

//PixelInput VS_Bone(VertexTextureNormal input)
//{
//    PixelInput output;
//    
//    output.position = GetWVP(input.position, Bones[BoneNumber]);
//    output.uv = input.uv;
//    output.normal = mul(input.normal, (float3x3)Bones[BoneNumber]);
//
//    return output;
//}

PixelInput VS(VertexTextureNormalTangentBlend input)
{
    PixelInput output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, Bones[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, Bones[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, Bones[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, Bones[(uint) input.blendIndices.w]);

    
    output.wPosition = GetWorld(input.position, transform);
    output.vPosition = GetView(output.wPosition);
    output.pPosition = GetProjection(output.vPosition);

    output.normal = mul(input.normal, (float3x3)transform);

    output.uv = input.uv;

    output.tangent = normalize(mul(input.tangent, (float3x3)transform));

    //정점에서 카메라로 나가도록
    output.view = normalize(GetViewPosition() - output.wPosition.xyz);

    return output;
}


float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    {
        float4 diffuseColor = 0;
        float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);
    
        float4 normalMap = _normalMap.Sample(_normalSampler, input.uv);
        float3 bump = TangentSpace(normalMap.rgb, input.normal, input.tangent);
    
        float3 color1 = 0;
        float3 color2 = 0;
        Diffuse(color1, input.normal);
        Diffuse(color2, diffuseMap.rgb, bump);
        //Diffuse(color2, diffuseMap.rgb, input.normal);
        color.rgb += color1 * 2.0f + color2 * 6.0f;
    }

    
    {
        float4 specularMap = _specularMap.Sample(_specularSampler, input.uv);
        //Specular(color.rgb, specularMap.rgb, input.normal, input.view);
        float3 sColor = 0;
        if (length(specularMap) > 0)
            Specular(sColor, input.normal, input.view);
        
        color.rgb += sColor * 1.0f;
    }

    //light
    {
        float3 plcolor = 0;
        PointLightingAll(plcolor, input.wPosition, input.normal);

        float3 slcolor = 0;
        SpotLightingAll(slcolor, input.wPosition, input.normal);

        color.rgb += plcolor + slcolor;
    }

    //float factor = 5.0f;
    //color *= factor;

    color.a = 1;
    return color;
}