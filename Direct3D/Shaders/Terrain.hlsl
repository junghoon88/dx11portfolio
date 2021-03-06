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

float4 AlphaDiffuse(float4 alpha1, float4 alpha2, float2 uv)
{
    float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, uv);
    float4 subDiffuse0 = subDiffuse[0].Sample(_diffuseSampler, uv);
    float4 subDiffuse1 = subDiffuse[1].Sample(_diffuseSampler, uv);
    float4 subDiffuse2 = subDiffuse[2].Sample(_diffuseSampler, uv);
    float4 subDiffuse3 = subDiffuse[3].Sample(_diffuseSampler, uv);
    float4 subDiffuse4 = subDiffuse[4].Sample(_diffuseSampler, uv);
    float4 subDiffuse5 = subDiffuse[5].Sample(_diffuseSampler, uv);

    if (any(alpha1.rgb + alpha2.rgb))
    {
        diffuseMap = (1 - alpha1.r) * diffuseMap + alpha1.r * subDiffuse0;
        diffuseMap = (1 - alpha1.g) * diffuseMap + alpha1.g * subDiffuse1;
        diffuseMap = (1 - alpha1.b) * diffuseMap + alpha1.b * subDiffuse2;
        diffuseMap = (1 - alpha2.r) * diffuseMap + alpha2.r * subDiffuse3;
        diffuseMap = (1 - alpha2.g) * diffuseMap + alpha2.g * subDiffuse4;
        diffuseMap = (1 - alpha2.b) * diffuseMap + alpha2.b * subDiffuse5;
    }

    return diffuseMap;
}

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

float3 AlphaNormal(float4 alpha1, float4 alpha2, float2 uv)
{
    float3 normalMap  = _normalMap.Sample(_normalSampler, uv).rgb;
    float3 subNormal0 = subNormal[0].Sample(_normalSampler, uv).rgb;
    float3 subNormal1 = subNormal[1].Sample(_normalSampler, uv).rgb;
    float3 subNormal2 = subNormal[2].Sample(_normalSampler, uv).rgb;
    float3 subNormal3 = subNormal[3].Sample(_normalSampler, uv).rgb;
    float3 subNormal4 = subNormal[4].Sample(_normalSampler, uv).rgb;
    float3 subNormal5 = subNormal[5].Sample(_normalSampler, uv).rgb;

    normalMap  = NormalFromColor(normalMap);
    subNormal0 = NormalFromColor(subNormal0);
    subNormal1 = NormalFromColor(subNormal1);
    subNormal2 = NormalFromColor(subNormal2);
    subNormal3 = NormalFromColor(subNormal3);
    subNormal4 = NormalFromColor(subNormal4);
    subNormal5 = NormalFromColor(subNormal5);

    
    //if (alpha1.r > 0.0f)    normalMap += alpha1.r * subNormal0;
    //if (alpha1.g > 0.0f)    normalMap += alpha1.g * subNormal1;
    //if (alpha1.b > 0.0f)    normalMap += alpha1.b * subNormal2;
    //if (alpha2.r > 0.0f)    normalMap += alpha2.r * subNormal3;
    //if (alpha2.g > 0.0f)    normalMap += alpha2.g * subNormal4;
    //if (alpha2.b > 0.0f)    normalMap += alpha2.b * subNormal5;

    if (any(alpha1.rgb + alpha2.rgb))
    {
        normalMap = (1 - alpha1.r) * normalMap + alpha1.r * subNormal0;
        normalMap = (1 - alpha1.g) * normalMap + alpha1.g * subNormal1;
        normalMap = (1 - alpha1.b) * normalMap + alpha1.b * subNormal2;
        normalMap = (1 - alpha2.r) * normalMap + alpha2.r * subNormal3;
        normalMap = (1 - alpha2.g) * normalMap + alpha2.g * subNormal4;
        normalMap = (1 - alpha2.b) * normalMap + alpha2.b * subNormal5;
    }



    normalMap = normalize(normalMap);

    float3 normal = 0;
    //normal.x = normalMap.x * 2 - 1.0f;
    //normal.y = normalMap.z * 2 - 1.0f;
    //normal.z = normalMap.y * 2 - 1.0f;

    normal = normalize(normalMap);

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
    float4 alphaDiffuse = AlphaDiffuse(input.alpha1, input.alpha2, input.uv);
    float3 alphaNormal  = AlphaNormal(input.alpha1, input.alpha2, input.uv);

    float4 diffuseColor = 0;

    Diffuse(diffuseColor.rgb, input.normal);


	// Z 픽셀 깊이를 균질 W 좌표로 나누어 픽셀의 깊이 값을 가져옵니다.
    float depthValue = input.pPosition.z / input.pPosition.w;

    //if (depthValue < 0.9f)
    {
        if (length(alphaNormal) > 0.0f)
        {
            float4 diffuseColor2 = 0;
            Diffuse(diffuseColor2.rgb, alphaDiffuse.rgb, alphaNormal);
            diffuseColor *= diffuseColor2;
            //diffuseColor = diffuseColor2;
        }
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