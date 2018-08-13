#include "__CommonHeader.hlsl"

cbuffer PS_Sky : register(b2)
{
    float4 HorizonColorTwilight;
    float4 HorizonColorDay;
    float4 HorizonColorNight;

    float4 CeilingColorTwilight;
    float4 CeilingColorDay;
    float4 CeilingColorNight;

    float Height; //수평선 높이
    float Time;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXTURE0;
    float4 oPosition : POSITION0;
};


//PixelInput VS(VertexTextureNormal input)
PixelInput VS(VertexTexture input)   //VertexTextureNormal로 넣어주는데 VertexTexture로 잘라서 받
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;
    output.oPosition = input.position;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 horizonColor = 0;
    float4 ceilingColor = 0;

    //하늘색
    if (_direction.y < 0)
    {
        //주간
        float amount = min(-1.0f * _direction.y * 1.5f, 1.0f);
        horizonColor = lerp(HorizonColorTwilight, HorizonColorDay, amount);
        ceilingColor = lerp(HorizonColorTwilight, CeilingColorDay, amount);
    }
    else
    {
        //야간
        float amount = min(_direction.y * 1.5f, 1.0f);
        horizonColor = lerp(HorizonColorTwilight, HorizonColorNight, amount);
        ceilingColor = lerp(HorizonColorTwilight, CeilingColorNight, amount);
    }

    float3 pixelColor = lerp(horizonColor, ceilingColor, saturate(input.oPosition.y / Height)) / _overcast;

    //태양 빛
    {
        float3 sunColor = float3(1.2f, 0.8f, 0.0f); //_suncolor;
        sunColor *= saturate(1 - distance(input.oPosition.xyz, _direction)) * 0.5f;
        sunColor *= (1 - _sunIntensity) / _overcast;
    
        pixelColor += sunColor;
    }

    //별
    {
        float horizonLerp = saturate(lerp(0.0f, 1.0f, input.oPosition.y * 1.5f));
        float directionLerp = lerp(0.0f, 1.0f, max(_direction.y, 0));
        float starAlpha = saturate(horizonLerp * directionLerp);

        float2 starUv = input.uv * 16;
        starUv.xy -= Time / 40;

        pixelColor += _diffuseMap.Sample(_diffuseSampler, starUv) * starAlpha * 0.6f / _overcast;
    }

    return float4(pixelColor, 1.0f);
}