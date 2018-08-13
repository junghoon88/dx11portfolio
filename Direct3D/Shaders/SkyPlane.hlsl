#include "__CommonHeader.hlsl"

Texture2D cloudTexture : register(t5);
Texture2D perturbTexture : register(t6);
SamplerState SampleType : register(s5);

cbuffer PS_SkyPlane : register(b2)
{
    float translation;
    float scale;
    float brightness;
    float padding;
};

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
    float4 perturbValue;
    float4 cloudColor;


	// 텍스처 좌표 샘플링 위치를 변환 값으로 변환합니다.
    input.uv.x = input.uv.x + translation;

	// 번역된 텍스처 좌표를 사용하여 섭동 텍스처에서 텍스처 값을 샘플링합니다.
    perturbValue = perturbTexture.Sample(SampleType, input.uv);

	// 섭동 값에 섭동 스케일을 곱합니다.
    perturbValue = perturbValue * scale;

	// 교란 된 텍스처 좌표 샘플링 위치를 얻기 위해 변환 값과 텍스처 좌표를 추가합니다.
    perturbValue.xy = perturbValue.xy + input.uv.xy + translation;

	// 이제 섭동 샘플링 좌표를 사용하여 구름 텍스처에서 색상을 샘플링합니다.
    cloudColor = cloudTexture.Sample(SampleType, perturbValue.xy);

	// 밝기 값으로 색상 구름을 줄입니다.
    cloudColor = cloudColor * brightness;

    return cloudColor;
}