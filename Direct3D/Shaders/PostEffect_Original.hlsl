#include "__CommonHeader.hlsl"

Texture2D RenderTarget : register(t5);
SamplerState RenderTargetSampler : register(s5);

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

    output.uv.x = 1.0f-input.uv.x;
    output.uv.y = input.uv.y;
    //output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return RenderTarget.Sample(RenderTargetSampler, input.uv);
}