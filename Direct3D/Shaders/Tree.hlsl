#include "__CommonHeader.hlsl"


cbuffer VS_Tree : register(b2)
{
    uint   FixedY;
    float3 Position;

    float3 Scale;
    float  Padding;
}


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


PixelInput VS(VertexTexture input)
{
    PixelInput output;

    matrix MatS = float4x4(float4(Scale.x, 0, 0, 0), float4(0, Scale.y, 0, 0), float4(0, 0, 1, 0), float4(0, 0, 0, 1));
    //matrix MatT = float4x4(float4(0, 0, 0, Position.x), float4(0, 0, 0, Position.y), float4(0, 0, 0, Position.z), float4(0, 0, 0, 1));
    matrix MatT = float4x4(float4(1, 0, 0, 0), float4(0, 1, 0, 0), float4(0, 0, 1, 0), float4(Position.x, Position.y, Position.z, 1));

    matrix MatR = _viewInverse;
    MatR[3][0] = 0;
    MatR[3][1] = 0;
    MatR[3][2] = 0;

    if (FixedY == 1)
    {
        MatR[0][1] = 0;
        MatR[1][0] = 0;
        MatR[2][1] = 0;
        MatR[1][2] = 0;
    }

    output.position = mul(input.position, _world);
    output.position = mul(output.position, MatS);
    output.position = mul(output.position, MatR);
    output.position = mul(output.position, MatT);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);
    
    clip(diffuseMap.a - 0.5f); //0이하는 안그린다.

    return diffuseMap;
}