#include "__CommonHeader.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

cbuffer PS_Color : register(b2)
{
    float4 _color : COLOR0;
    uint _viewMode : MODE0;
    float3 padding : PADDING0;
}



PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    
    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float4 diffuse = GetDiffuseColor(_diffuse, _direction, input.normal);

    //diffuse = (diffuse * 0.3f) + 0.7f;

    //float4 _ambient;
    //float4 _diffuse;
    //float4 _specular;
    //float4 _emissive;
    //float _shininess;
    //float _detailFactor;

    float4 color = _color;
    if (_viewMode == 1)         color = float4(1, 1, 0, 1);


    return diffuse;
}