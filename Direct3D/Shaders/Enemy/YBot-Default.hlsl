#include "../__CommonHeader.hlsl"

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
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

PixelInput VS(VertexTextureNormalTangentBlend input)
{
    PixelInput output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, Bones[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, Bones[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, Bones[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, Bones[(uint) input.blendIndices.w]);

    
    output.position = GetWVP(input.position, transform);

    output.normal = mul(input.normal, (float3x3) transform);

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 diffuseColor = _diffuseMap.Sample(_diffuseSampler, input.uv);
    if (length(diffuseColor) <= 0.0f)
    {
        //color.rgb = _diffuse.rgb;
        Diffuse(color.rgb, _diffuse.rgb, input.normal);
    }
	else
    {
        Diffuse(color.rgb, diffuseColor.rgb, input.normal);
    }


    float4 specularColor = _specularMap.Sample(_specularSampler, input.uv);
    if (length(specularColor) <= 0.0f)
        specularColor = _specular;
    Specular(color.rgb, specularColor.rgb, input.normal);


    color.a = 1;

    return color;
}