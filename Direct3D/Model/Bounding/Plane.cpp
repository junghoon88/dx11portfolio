#include "stdafx.h"
#include "Plane.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingFrustum.h"

PlaneIntersectionType Plane::Intersects(const BoundingBox* box)
{
	D3DXVECTOR3 vector3  = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 vector31 = D3DXVECTOR3(0, 0, 0);

	D3DXVECTOR3 boxMin = box->GetMin();
	D3DXVECTOR3 boxMax = box->GetMax();

	vector31.x = (Normal.x >= 0.0f ? boxMin.x : boxMax.x);
	vector31.y = (Normal.y >= 0.0f ? boxMin.y : boxMax.y);
	vector31.z = (Normal.z >= 0.0f ? boxMin.z : boxMax.z);
	vector3.x  = (Normal.x >= 0.0f ? boxMax.x : boxMin.x);
	vector3.y  = (Normal.y >= 0.0f ? boxMax.y : boxMin.y);
	vector3.z  = (Normal.z >= 0.0f ? boxMax.z : boxMin.z);
	if (Normal.x * vector31.x + Normal.y * vector31.y + Normal.z * vector31.z + D > 0.0f)
	{
		return PlaneIntersectionType::Front;
	}
	if (Normal.x * vector3.x + Normal.y * vector3.y + Normal.z * vector3.z + D < 0.0f)
	{
		return PlaneIntersectionType::Back;
	}
	return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType Plane::Intersects(const BoundingFrustum* plane)
{
	float single;
	int num = 0;
	for (int i = 0; i < 8; i++)
	{
		single = D3DXVec3Dot(&plane->cornerArray[i], &Normal);
		if (single + D <= 0.0f)
		{
			num |= 2;
		}
		else
		{
			num |= 1;
		}
		if (num == 3)
		{
			return PlaneIntersectionType::Intersecting;
		}
	}
	if (num != 1)
	{
		return PlaneIntersectionType::Back;
	}
	return PlaneIntersectionType::Front;
}


PlaneIntersectionType Plane::Intersects(const BoundingSphere* sphere)
{
	float x = sphere->Center.x * Normal.x + sphere->Center.y * Normal.y + sphere->Center.z * Normal.z;
	float d = x + D;
	if (d > sphere->Radius)
	{
		return PlaneIntersectionType::Front;
	}
	if (d < -sphere->Radius)
	{
		return PlaneIntersectionType::Back;
	}
	return PlaneIntersectionType::Intersecting;
}