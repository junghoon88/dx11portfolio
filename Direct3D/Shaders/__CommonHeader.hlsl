cbuffer VS_ViewProjection : register(b0)
{
    matrix _view;
    matrix _projection;
    matrix _viewInverse;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer VS_Time : register(b10)
{
    float _elapsedTime;
    float _runningTime;
}

cbuffer PS_Light : register(b0)
{
    float3 _direction;
    float  Lightpadding1;

    float4 _suncolor;

    float  _overcast;
    float  _sunIntensity;
    //float2 Lightpadding2; //없어도됨
}

cbuffer PS_Material : register(b1)
{
    float4 _ambient;
    float4 _diffuse;
    float4 _specular;
    float4 _emissive;

    float _shininess;
    float _detailFactor;

    float MaterialPadding[2];
}

Texture2D _diffuseMap : register(t0);
Texture2D _specularMap : register(t1);
Texture2D _emissiveMap : register(t2);
Texture2D _normalMap : register(t3);
Texture2D _detailMap : register(t4);

SamplerState _diffuseSampler : register(s0);
SamplerState _specularSampler : register(s1);
SamplerState _emissiveSampler : register(s2);
SamplerState _normalSampler : register(s3);
SamplerState _detailSampler : register(s4);


struct Vertex
{
    float4 position : POSITION0;
};

struct VertexColor
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct VertexColorNormal
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float3 normal : NORMAL0;
};

struct VertexTexture
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VertexTextureNormal
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexColorTextureNormal
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexTextureNormalBlend
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 blendIndices : BLENDINDICES0;
    float4 blendWeights : BLENDWEIGHTS0;
};

struct VertexTextureNormalTangentBlend
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 blendIndices : BLENDINDICES0;
    float4 blendWeights : BLENDWEIGHTS0;
    float3 tangent : TANGENT0;
};

struct VertexTypePTNC2
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 alpha1 : COLOR0;
    float4 alpha2 : COLOR1;
};


float3 GetViewPosition()
{
    return _viewInverse._41_42_43;
}

float4 GetWorld(float4 position)
{
    position = mul(position, _world);
    return position;
}

float4 GetWorld(float4 position, float4x4 world)
{
    position = mul(position, world);
    return position;
}

float4 GetView(float4 wPosition)
{
    wPosition = mul(wPosition, _view);
    return wPosition;
}

float4 GetProjection(float4 vPosition)
{
    vPosition = mul(vPosition, _projection);
    return vPosition;
}

float4 GetWV(float4 position)
{
    position = GetWorld(position);
    position = GetView(position);
    return position;
}

float4 GetWV(float4 position, float4x4 world)
{
    position = GetWorld(position, world);
    position = GetView(position);
    return position;
}

float4 GetWVP(float4 position)
{
    position = GetWorld(position);
    position = GetView(position);
    position = GetProjection(position);
    return position;
}

float4 GetVP(float4 wPosition)
{
    wPosition = GetView(wPosition);
    wPosition = GetProjection(wPosition);
    return wPosition;
}

float3 GetWorldNormal(float3 normal)
{
    normal = mul(normal, (float3x3) _world);
    return normal;
}

float3 GetWorldNormal(float3 normal, float3x3 world)
{
    normal = mul(normal, world);
    return normal;
}


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


struct SpotLight
{
    float3 Position;
    float  InnerAngle;
    float3 Color;
    float  OuterAngle;
    float3 Direction;
    float  Range;
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
    float lightAngle = dot(centerDir, lightDir); //cos(angle) 값

    float intensity = 0;

	[branch] //if - else 중 하나만 계산함. (안하면 둘다 계산함)
    if (lightAngle > 0.0f)
    {
        [branch]
        if (lightAngle < light.OuterAngle) 
        {
            //외각보다 밖에 있으므로 0
            intensity = 0.0f;
        }
        else if (lightAngle < light.InnerAngle)
        {
            //내각과 외각 사이에 있을 경우
            float s = (lightAngle - light.OuterAngle) / (light.InnerAngle - light.OuterAngle);
            intensity = smoothstep(light.OuterAngle, light.InnerAngle, saturate(s));
        }
        else
        {
            //내각보다 안에 있으므로 1
            intensity = 1.0f;
        }
    }

    color += light.Color * intensity;

}

////////////////////////////////


cbuffer PS_Fog : register(b12)
{
    float4 _fogColor;
    float _fogStart;
    float _fogEnd;
}

void LinearFog(inout float3 color, float4 vPosition)
{
    float factor = saturate((_fogEnd - vPosition.z) / (_fogEnd - _fogStart));

    //color = lerp()
    color = (1.0f - factor) * _fogColor.rgb + factor * color;
}


////////////////////////////////

void Diffuse(inout float3 color, float3 direction, float3 normal)
{
    float3 light = -direction;
    float intensity = saturate(dot(normal, light));

    color = color + _diffuse.rgb * intensity;
}

void Diffuse(inout float3 color, float3 diffuse, float3 direction, float3 normal)
{
    float3 light = -direction;
    float intensity = saturate(dot(normal, light));

    color = color + diffuse * intensity;
}

void Specular(inout float3 color, float3 direction, float3 normal, float3 eye)
{
    float3 reflection = reflect(_direction, normal); //반사광 계산
    float intensity = saturate(dot(reflection, eye)); //반사광과 카메라를 바라보는 방향의 내적 -> 시야각
    float specular = pow(intensity, _shininess);

    color = color + _specular.rgb * specular;
}

void Specular(inout float3 color, float3 mapIntensity, float3 direction, float3 normal, float3 eye)
{
    float3 reflection = reflect(_direction, normal); //반사광 계산
    float intensity = saturate(dot(reflection, eye)); //반사광과 카메라를 바라보는 방향의 내적 -> 시야각
    float specular = pow(intensity, _shininess);

    color = color + _specular.rgb * mapIntensity * specular;
}
//디퓨즈 음영강도 계산
float GetDiffuseIntensity(float3 direction, float3 normal)
{
    float3 light = direction * -1;
    
    return saturate(dot(normal, light));
}

//디퓨즈 라이팅 계산
float4 GetDiffuseColor(float4 color, float3 direction, float3 normal)
{
    float3 light = direction * -1;
    float intensity = saturate(dot(normal, light));

    return float4(color.rgb * intensity, 0);
}
