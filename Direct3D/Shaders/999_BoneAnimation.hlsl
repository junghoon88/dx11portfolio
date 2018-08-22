#include "__CommonHeader.hlsl"

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

PixelInput VS_Bone(VertexTextureNormal input)
{
    PixelInput output;
    
    output.position = GetWVP(input.position, Bones[BoneNumber]);

    output.uv = input.uv;

    output.normal = mul(input.normal, (float3x3)Bones[BoneNumber]);

    return output;
}

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
    //float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    //return GetDiffuseColor(color, _direction, input.normal);

    return _diffuseMap.Sample(_diffuseSampler, input.uv);
}