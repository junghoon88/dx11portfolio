#include "__CommonHeader.hlsl"
#include "_LightHeader.hlsl"
#include "_NormalHeader.hlsl"

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

struct EnemyArea
{
    float2 Start;
    float2 Size;
    float4 Color;
};

cbuffer PS_EnemyArea : register(b3)
{
    EnemyArea enemyArea[10];
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 oPosition : POSITION0;
    float4 wPosition : POSITION1;
    float4 pPosition : POSITION2;

    float4 alpha1 : COLOR1;
    float4 alpha2 : COLOR2;

};

Texture2D subDiffuse[6] : register(t5);
Texture2D subNormal[6] : register(t11);

PixelInput VS(VertexTypePTNC2 input)
{
    PixelInput output;

    output.oPosition = input.position;
    output.wPosition = GetWorld(input.position);
    float4 vPosition = GetView(output.wPosition);
    output.position  = GetProjection(vPosition);
    output.pPosition = output.position;

    output.normal = GetWorldNormal(input.normal, (float3x3)_world);
    
    output.uv = input.uv;


    output.alpha1 = input.alpha1;
    output.alpha2 = input.alpha2;


    return output;
}

/////////////////////////////////////////////////////////////////////////

float3 NormalFromColor(float3 color)
{
    if (length(color) == 0.0f)
        return float3(0, 0, 0);

    float3 normal;
    normal.x = color.x * 2 - 1.0f;
    normal.y = color.z * 2 - 1.0f;
    normal.z = color.y * 2 - 1.0f;
    return normal;
}

float4 BrushColor(float4 oPosition)
{
    float4 overColor = float4(1, 1, 1, 1);
    if (BrushType == 1)
    {
        float dist = sqrt(pow(oPosition.x - Position.x, 2)
                        + pow(oPosition.z - Position.y, 2));

        if (dist < Radius)
        {
            overColor = ColorHover;
        }
    }
    if (BrushType == 2)
    {
        if (oPosition.x >= HoverGridStart.x && oPosition.x < HoverGridStart.x + HoverGridSize.x
            && oPosition.z >= HoverGridStart.y && oPosition.z < HoverGridStart.y + HoverGridSize.y)
        {
            overColor = ColorHover;
        }
        else if (oPosition.x >= SelectGridStart.x && oPosition.x < SelectGridStart.x + SelectGridSize.x
            && oPosition.z >= SelectGridStart.y && oPosition.z < SelectGridStart.y + SelectGridSize.y)
        {
            overColor = ColorSelected;
        }
    }

    return overColor;
}

float4 AreaColor(float4 oPosition)
{
    float4 areaColor = float4(1, 1, 1, 1);
    for (int i = 0; i < 10; i++)
    {
        if (length(enemyArea[i].Size) > 0.0f)
        {
            if (oPosition.x >= enemyArea[i].Start.x && oPosition.x < enemyArea[i].Start.x + enemyArea[i].Size.x
            && oPosition.z >= enemyArea[i].Start.y && oPosition.z < enemyArea[i].Start.y + enemyArea[i].Size.y)
            {
                areaColor = enemyArea[i].Color;
            }
        }
    }
    return areaColor;
}

float4 ShowGrid(float4 color, float4 oPosition)
{
    if (bView == 1)
    {
        float2 grid = oPosition.xz / (float) GridSpacing;

        float2 range = abs(frac(grid - 0.5f) - 0.5f); //0.5~0~0.5 -> 그리드 기준으로 양방향으로 두꺼워지도록
        float2 speed = fwidth(grid); // fwidth(grid) = abs(ddx(grid)) + abs(ddy(grid)) -> 버텍스수/픽셀 (픽셀단위로 미분했을 때 버텍스 증가량)
                                        // 인접한 pixel 

        float2 pixelRange = range / speed;
        float lineWeight = saturate(min(pixelRange.x, pixelRange.y) - GridThickness);

        return lerp(ColorGrid, color, lineWeight);
    }
    return color;
}


float4 PS(PixelInput input) : SV_TARGET
{
    float4 alphaDiffuse = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float3 alphaNormal = _normalMap.Sample(_normalSampler, input.uv).rgb;
    alphaNormal = NormalFromColor(alphaNormal);
    alphaNormal = normalize(alphaNormal);

    float4 diffuseColor = 0;

    Diffuse(diffuseColor.rgb, input.normal);

    if (length(alphaNormal) > 0.0f)
    {
        float4 diffuseColor2 = 0;
        Diffuse(diffuseColor2.rgb, alphaDiffuse.rgb, alphaNormal);
        diffuseColor *= diffuseColor2;
        //diffuseColor = diffuseColor2;
    }
    diffuseColor.a = 1;
    float4 outColor = alphaDiffuse * diffuseColor;

    //Mouse Hovering, Select Color
    float4 brushColor = BrushColor(input.oPosition);
    //EnemyArea Color
    float4 areaColor = AreaColor(input.oPosition);

    outColor *= brushColor * areaColor;

    //Lights
    PointLightingAll(outColor.rgb, input.wPosition, input.normal);
    SpotLightingAll(outColor.rgb, input.wPosition, input.normal);



    //grid line render
    outColor = ShowGrid(outColor, input.oPosition);

    return outColor;
}