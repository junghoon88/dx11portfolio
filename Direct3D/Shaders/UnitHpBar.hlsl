#include "__CommonHeader.hlsl"

#define SCREEN_WIDTH  1280.0f
#define SCREEN_HEIGHT 720.0f

cbuffer VS_HPBar : register(b5)
{
    float hpbar_width;
    float hpbar_height;
    float hpbar_yOffset;
    float hpbar_hpRatio;

    float4 hpbar_colorHP;
    float4 hpbar_colorBack;
}


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float hpRatio : RATIO0;

};

PixelInput VS(VertexTexture input)
{
    PixelInput output;

    input.position.y += hpbar_yOffset;
    float4 center = GetWVP(input.position);
    center.xyz = center.xyz / center.w;

    output.position.x = center.x + (input.uv.x - 0.5f) * 2.0f * hpbar_width / SCREEN_WIDTH;
    output.position.y = center.y + (input.uv.y - 0.5f) * 2.0f * hpbar_height / SCREEN_HEIGHT;
    output.position.z = center.z;
    output.position.w = 1.0f;

    output.hpRatio = hpbar_hpRatio;
    
    output.uv = input.uv;

    return output;
}

Texture2D hpBarTexture[2] : register(t5);


float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    if (input.uv.x <= input.hpRatio)
    {
        color = hpBarTexture[0].Sample(_diffuseSampler, input.uv);
    }
    else
    {
        color = hpBarTexture[1].Sample(_diffuseSampler, input.uv);
    }
    color.a = 1.0f;

    return color;
}