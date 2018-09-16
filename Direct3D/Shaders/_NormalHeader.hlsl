#pragma once
float3 TangentSpace(float3 normalMap, float3 normal, float3 tangent)
{
    //0~1 -> -1~1 로 변경
    float3 coord = 2.0f * normalMap - 1.0f;

    float3 N = normal; //Z
    float3 T = normalize(tangent - dot(tangent, N) * N); //X
    float3 B = cross(N, T); //Y

    float3x3 TBN = float3x3(T, B, N);
    
    return mul(coord, TBN);
}

float3 BumpNormal(float3 normalMap, float3 normal, float3 tangent)
{
    float3 biNormal = cross(normal, tangent);
    normalMap = normalMap * 2.0f - 1.0f;

    float3 bumpNormal = (normalMap.x * tangent) + (normalMap.y * biNormal) + (normalMap.z * normal);

    bumpNormal = normalize(bumpNormal);
    return bumpNormal;

    //0~1 -> -1~1 로 변경
    //float3 coord = 2.0f * normalMap - 1.0f;

    //float3 N = normal; //Z
    //float3 T = normalize(tangent - dot(tangent, N) * N); //X
    //float3 B = cross(N, T); //Y

    //float3x3 TBN = float3x3(T, B, N);
    
    //return mul(coord, TBN);
}
