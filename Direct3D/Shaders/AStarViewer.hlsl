#include "__CommonHeader.hlsl"


struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};


//PixelInput VS(VertexTextureNormal input)
PixelInput VS(VertexColor input)   //VertexTextureNormal�� �־��ִµ� VertexTexture�� �߶� ��
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.color = input.color;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return input.color;

}