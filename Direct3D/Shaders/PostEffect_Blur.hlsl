#include "__CommonHeader.hlsl"

Texture2D RenderTarget : register(t5);
SamplerState RenderTargetSampler : register(s5);

cbuffer PS_Light : register(b2)
{
    uint Count;
    float Width;
    float Height;
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

static const int BlurSize = 20;
float4 PS(PixelInput input) : SV_TARGET
{
    float x = input.uv.x;
    float y = input.uv.y;

    float3 color = RenderTarget.Sample(RenderTargetSampler, float2(x, y));


    float2 arr[BlurSize];
    for (uint i = 0; i < Count; i++)
    {
        //arr[i] = float2(0, 0);
        arr[i].x = (float) (i + 1) / Width;
        arr[i].y = (float) (i + 1) / Height;
    }

    float2 temp = float2(0, 0);
    for (i = 0; i < Count; i++)
    {
        temp = float2(x - arr[i].x, y);
        color += RenderTarget.Sample(RenderTargetSampler, saturate(temp));

        temp = float2(x + arr[i].x, y);
        color += RenderTarget.Sample(RenderTargetSampler, saturate(temp));

        temp = float2(x, y - arr[i].y);
        color += RenderTarget.Sample(RenderTargetSampler, saturate(temp));

        temp = float2(x, y + arr[i].y);
        color += RenderTarget.Sample(RenderTargetSampler, saturate(temp));
    }
    color = color / (Count * 4 + 1);

    return float4(color, 1);
}


float4 PS2(PixelInput input) : SV_TARGET
{
    float2 arr[9] =
    {
        float2(-1, -1), float2(0, -1), float2(1, -1), 
        float2(-1,  0), float2(0,  0), float2(1,  0),
        float2(-1,  1), float2(0,  1), float2(1,  1)
    };

    float3 color = float3(0, 0, 0);
    for (uint i = 0; i < 9; i++)
    {
        float x = arr[i].x / Width;
        float y = arr[i].y / Height;

        float2 uv = input.uv + float2(x, y);
        color += RenderTarget.Sample(RenderTargetSampler, saturate(uv));
    }
    color /= 9.0f;

    return float4(color, 1);

}