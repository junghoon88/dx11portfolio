#include "__CommonHeader.hlsl"

Texture2D RenderTarget : register(t5);
SamplerState RenderTargetSampler : register(s5);

cbuffer PS_Light : register(b2)
{
    uint Select;
}

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

    output.uv = float2(1 - input.uv.x, input.uv.y);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float3 color = RenderTarget.Sample(RenderTargetSampler, input.uv);

    if (Select == 0)
    {
        float avg = (color.r + color.g + color.b) / 3;

        return float4(avg, avg, avg, 1);
    }

    float3 factor = float3(0.298f, 0.587f, 0.114f);
    if (Select == 1)
    {
        float c = dot(color, factor);

        return float4(c, c, c, 1);
    }

    return float4(1, 1, 1, 1);

}