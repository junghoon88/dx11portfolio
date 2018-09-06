#include "stdafx.h"
#include "FrustumCulling.h"
#include "Camera.h"
#include "Perspective.h"

FrustumCulling::FrustumCulling(float zFar, Camera* camera, Perspective* perspective)
	: zFar(zFar), camera(camera), perspective(perspective)
{
}

FrustumCulling::~FrustumCulling()
{
}

void FrustumCulling::Update(void)
{
	D3DXMATRIX V, P;
	camera->GetMatrix(&V);
	perspective->GetMatrix(&P);

	float zNear = -P._43 / P._33;
	float radius = zFar / (zFar - zNear);

	P._33 = radius;
	P._43 = -radius * zNear;

	D3DXMATRIX matrix;
	matrix = V * P;
	D3DXMatrixMultiply(&matrix, &V, &P);

	//가까운 평면(가까우면 -여야 하지만 카메라는 반대로(?))
	{
		//		  변환될 위치 + Z축
		planes[0].a = matrix._14 + matrix._13;
		planes[0].b = matrix._24 + matrix._23;
		planes[0].c = matrix._34 + matrix._33;
		planes[0].d = matrix._44 + matrix._43;
	}

	//먼 평면
	{
		planes[1].a = matrix._14 - matrix._13;
		planes[1].b = matrix._24 - matrix._23;
		planes[1].c = matrix._34 - matrix._33;
		planes[1].d = matrix._44 - matrix._43;
	}

	//왼쪽 평면(마찬가지로 카메라라서 +)
	{
		planes[2].a = matrix._14 + matrix._11;
		planes[2].b = matrix._24 + matrix._21;
		planes[2].c = matrix._34 + matrix._31;
		planes[2].d = matrix._44 + matrix._41;
	}

	//오른쪽 평면
	{
		planes[3].a = matrix._14 - matrix._11;
		planes[3].b = matrix._24 - matrix._21;
		planes[3].c = matrix._34 - matrix._31;
		planes[3].d = matrix._44 - matrix._41;
	}

	//위 평면(마찬가지로 카메라라서 -)
	{
		planes[4].a = matrix._14 - matrix._12;
		planes[4].b = matrix._24 - matrix._22;
		planes[4].c = matrix._34 - matrix._32;
		planes[4].d = matrix._44 - matrix._42;
	}

	//아래 평면
	{
		planes[5].a = matrix._14 + matrix._12;
		planes[5].b = matrix._24 + matrix._22;
		planes[5].c = matrix._34 + matrix._32;
		planes[5].d = matrix._44 + matrix._42;
	}

	for (size_t i = 0; i < 6; i++)
	{
		D3DXPlaneNormalize(&planes[i], &planes[i]);
	}
}

bool FrustumCulling::ContainPoint(float x, float y, float z)
{
	return ContainPoint(D3DXVECTOR3(x, y, z));
}

bool FrustumCulling::ContainPoint(D3DXVECTOR3 position)
{
	//6개의 평면과 점을 내적해서 모두 양수가 나오면 내부에 있는것으로 판단
	for (size_t i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &position) < 0.0f)
			return false;
	}

	return true;
}
