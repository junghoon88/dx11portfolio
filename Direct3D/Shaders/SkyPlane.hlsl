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


	// �ؽ�ó ��ǥ ���ø� ��ġ�� ��ȯ ������ ��ȯ�մϴ�.
    input.uv.x = input.uv.x + translation;

	// ������ �ؽ�ó ��ǥ�� ����Ͽ� ���� �ؽ�ó���� �ؽ�ó ���� ���ø��մϴ�.
    perturbValue = perturbTexture.Sample(SampleType, input.uv);

	// ���� ���� ���� �������� ���մϴ�.
    perturbValue = perturbValue * scale;

	// ���� �� �ؽ�ó ��ǥ ���ø� ��ġ�� ��� ���� ��ȯ ���� �ؽ�ó ��ǥ�� �߰��մϴ�.
    perturbValue.xy = perturbValue.xy + input.uv.xy + translation;

	// ���� ���� ���ø� ��ǥ�� ����Ͽ� ���� �ؽ�ó���� ������ ���ø��մϴ�.
    cloudColor = cloudTexture.Sample(SampleType, perturbValue.xy);

	// ��� ������ ���� ������ ���Դϴ�.
    cloudColor = cloudColor * brightness;

    return cloudColor;
}