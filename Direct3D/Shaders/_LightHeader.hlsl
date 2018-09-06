#pragma once
///////////////////////////////
// Lights
struct PointLight
{
    float3 Position;
    float Intensity;
    float3 Color;
    float Range;
};

cbuffer PS_PointLights : register(b10)
{
    PointLight _pointLights[32];

    int _pointLightCount;
}

void PointLighting(inout float3 color, PointLight light, float4 wPosition, float3 normal)
{
    float dist = distance(light.Position, wPosition.xyz);
    float intensity = saturate((light.Range - dist) / light.Range);
    intensity = pow(intensity, 2) * light.Intensity;
    color += light.Color * intensity;
}

void PointLightingAll(inout float3 color, float4 wPosition, float3 normal)
{
    for (int i = 0; i < _pointLightCount; i++)
    {
        PointLighting(color, _pointLights[i], wPosition, normal);
    }
}

///////////////////////////////

struct SpotLight
{
    float3 Position;
    float InnerAngle;
    float3 Color;
    float OuterAngle;
    float3 Direction;
    float Range;
};

cbuffer PS_SpotLight : register(b11)
{
    SpotLight _spotLights[32];

    int _spotLightCount;
}

void SpotLighting(inout float3 color, SpotLight light, float4 wPosition, float3 normal)
{
    float3 lightDir = normalize(wPosition.xyz - light.Position);
    float3 centerDir = normalize(light.Direction);
    float lightAngle = dot(centerDir, lightDir); //cos(angle) ��

    float intensity = 0;

	[branch] //if - else �� �ϳ��� �����. (���ϸ� �Ѵ� �����)
    if (lightAngle > 0.0f)
    {
        [branch]
        if (lightAngle < light.OuterAngle)
        {
            //�ܰ����� �ۿ� �����Ƿ� 0
            intensity = 0.0f;
        }
        else if (lightAngle < light.InnerAngle)
        {
            //������ �ܰ� ���̿� ���� ���
            float s = (lightAngle - light.OuterAngle) / (light.InnerAngle - light.OuterAngle);
            intensity = smoothstep(light.OuterAngle, light.InnerAngle, saturate(s));
        }
        else
        {
            //�������� �ȿ� �����Ƿ� 1
            intensity = 1.0f;
        }
    }

    color += light.Color * intensity;

}

void SpotLightingAll(inout float3 color, float4 wPosition, float3 normal)
{
    for (int i = 0; i < _spotLightCount; i++)
    {
        SpotLighting(color.rgb, _spotLights[i], wPosition, normal);
    }
}

///////////////////////////////