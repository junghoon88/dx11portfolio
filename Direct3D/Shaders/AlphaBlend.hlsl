#include "__CommonHeader.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXTURE0;
};


PixelInput VS(VertexTexture input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return _diffuseMap.Sample(_diffuseSampler, input.uv);
}