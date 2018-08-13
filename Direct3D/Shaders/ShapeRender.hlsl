#include "__CommonHeader.hlsl"

//cbuffer PS_Light : register(b2)
//{
//    int2 DragStart;
//    int2 DragSize;
//    bool bDragging;
//}

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};


PixelInput VS(VertexColor input)
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

PixelInput VS_DRAG(VertexColor input)
{
    PixelInput output;

    output.position = input.position;
    output.color = input.color;

    return output;
}

float4 PS_DRAG(PixelInput input) : SV_TARGET
{
    //if(input.position.x >= DragStart.x
    //    && input.position.x <= DragStart.x + DragSize.x
    //    && input.position.y >= DragStart.y
    //    && input.position.y <= DragStart.y + DragSize.y)
    //{
    //    return (input.color * float4(0, 1, 0, 1));
    //}

    //return input.color;
    return input.color;

}