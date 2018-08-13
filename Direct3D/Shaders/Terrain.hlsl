#include "__CommonHeader.hlsl"

cbuffer PS_Grid : register(b2)
{
    uint    bView;
    uint    GridSpacing;
    float   GridThickness;
    float   PaddingGrid0;

    //brush
    uint    BrushType;
    float   Radius;
    float2  Position;

    //grid
    float2 HoverGridStart;
    float2 HoverGridSize;
    float2 SelectGridStart;
    float2 SelectGridSize;

    //Color
    float4  ColorGrid;

    float4  ColorHover;
    float4  ColorSelected;
}


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 oPosition : POSITION0;
    float4 wPosition : POSITION1;

    float4 alpha1 : COLOR1;
    float4 alpha2 : COLOR2;

};

Texture2D subTextures[6] : register(t5);

PixelInput VS(VertexTypePTNC2 input)
{
    PixelInput output;

    output.oPosition = input.position;
    output.wPosition = GetWorld(input.position);
    float4 vPosition = GetView(output.wPosition);
    output.position  = GetProjection(vPosition);

    output.normal = GetWorldNormal(input.normal, (float3x3)_world);
    
    output.uv = input.uv;


    output.alpha1 = input.alpha1;
    output.alpha2 = input.alpha2;


    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //test
    float4 testColor = float4(0, 0, 0, 1);
    int i = 0;
    for (i = 0; i < _pointLightCount; i++)
        PointLighting(testColor.rgb, _pointLights[i], input.wPosition, input.normal);

    for (i = 0; i < _spotLightCount; i++)
        SpotLighting(testColor.rgb, _spotLights[i], input.wPosition, input.normal);
    
    return testColor;



    float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float4 subDiffuse0 = subTextures[0].Sample(_diffuseSampler, input.uv);
    float4 subDiffuse1 = subTextures[1].Sample(_diffuseSampler, input.uv);
    float4 subDiffuse2 = subTextures[2].Sample(_diffuseSampler, input.uv);
    float4 subDiffuse3 = subTextures[3].Sample(_diffuseSampler, input.uv);
    float4 subDiffuse4 = subTextures[4].Sample(_diffuseSampler, input.uv);
    float4 subDiffuse5 = subTextures[5].Sample(_diffuseSampler, input.uv);

     
    //float4 test = diffuseMap;

    if (any(input.alpha1.rgb + input.alpha2.rgb))
    {
        //float sum = input.alpha1.r + input.alpha1.g + input.alpha1.b + input.alpha2.r + input.alpha2.g + input.alpha2.b;
        //input.alpha1 /= sum;
        //input.alpha2 /= sum;
        //diffuseMap =  float4(subDiffuse0 * input.alpha1.r)
        //            + float4(subDiffuse1 * input.alpha1.g)
        //            + float4(subDiffuse2 * input.alpha1.b)
        //            + float4(subDiffuse3 * input.alpha2.r)
        //            + float4(subDiffuse4 * input.alpha2.g)
        //            + float4(subDiffuse5 * input.alpha2.b);

        diffuseMap = (1 - input.alpha1.r) * diffuseMap + input.alpha1.r * subDiffuse0;
        diffuseMap = (1 - input.alpha1.g) * diffuseMap + input.alpha1.g * subDiffuse1;
        diffuseMap = (1 - input.alpha1.b) * diffuseMap + input.alpha1.b * subDiffuse2;
        diffuseMap = (1 - input.alpha2.r) * diffuseMap + input.alpha2.r * subDiffuse3;
        diffuseMap = (1 - input.alpha2.g) * diffuseMap + input.alpha2.g * subDiffuse4;
        diffuseMap = (1 - input.alpha2.b) * diffuseMap + input.alpha2.b * subDiffuse5;

    }
    
    //Mouse Hovering, Select Color
    float4 overColor = float4(1, 1, 1, 1);
    if (BrushType == 1)
    {
        float dist = sqrt(pow(input.oPosition.x - Position.x, 2)
                        + pow(input.oPosition.z - Position.y, 2));

        if (dist < Radius)
        {
            overColor = ColorHover;
        }
    }
    if (BrushType == 2)
    {
        if (input.oPosition.x >= HoverGridStart.x && input.oPosition.x < HoverGridStart.x + HoverGridSize.x
            && input.oPosition.z >= HoverGridStart.y && input.oPosition.z < HoverGridStart.y + HoverGridSize.y)
        {
            overColor = ColorHover;
        }
        else if (input.oPosition.x >= SelectGridStart.x && input.oPosition.x < SelectGridStart.x + SelectGridSize.x
            && input.oPosition.z >= SelectGridStart.y && input.oPosition.z < SelectGridStart.y + SelectGridSize.y)
        {
            overColor = ColorSelected;
        }
    }

    float4 diffuse = GetDiffuseColor(_diffuse, _direction, input.normal);

    float4 outColor = diffuseMap * diffuse * overColor;

    //int i = 0;
    //for (i = 0; i < _pointLightCount; i++)
    //    PointLighting(outColor.rgb, _pointLights[i], input.wPosition, input.normal);
    //
    //for (i = 0; i < _spotLightCount; i++)
    //    SpotLighting(outColor.rgb, _spotLights[i], input.wPosition, input.normal);


    //grid line render
    if (bView == 1)
    {
        float2 grid = input.oPosition.xz / (float)GridSpacing;

        float2 range = abs(frac(grid - 0.5f) - 0.5f); //0.5~0~0.5 -> 그리드 기준으로 양방향으로 두꺼워지도록
        float2 speed = fwidth(grid);    // fwidth(grid) = abs(ddx(grid)) + abs(ddy(grid)) -> 버텍스수/픽셀 (픽셀단위로 미분했을 때 버텍스 증가량)
                                        // 인접한 pixel 

        float2 pixelRange = range / speed;
        float lineWeight = saturate(min(pixelRange.x, pixelRange.y) - GridThickness);

        return lerp(ColorGrid, outColor, lineWeight);
    }


    return outColor;
}