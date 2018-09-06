#include "__CommonHeader.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    output.normal = mul(input.normal, (float3x3)_world);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 diffuseColor = _diffuseMap.Sample(_diffuseSampler, input.uv);
    if (length(diffuseColor) <= 0.0f)
        diffuseColor = _diffuse;

    Diffuse(color.rgb, diffuseColor.rgb, input.normal);

    color.a = 1;

    return color;
}