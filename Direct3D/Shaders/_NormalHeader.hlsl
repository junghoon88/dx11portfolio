#pragma once
float3 TangentSpace(float3 normalMap, float3 normal, float3 tangent)
{
    //0~1 -> -1~1 ·Î º¯°æ
    float3 coord = 2.0f * normalMap - 1.0f;

    float3 N = normal; //Z
    float3 T = normalize(tangent - dot(tangent, N) * N); //X
    float3 B = cross(N, T); //Y

    float3x3 TBN = float3x3(T, B, N);
    
    return mul(coord, TBN);
}
