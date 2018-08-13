#include "stdafx.h"
#include "BoundingFrustum.h"

#include "Ray.h"
#include "Plane.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"

#include "../../Utilities/Math.h"

BoundingFrustum::BoundingFrustum()
	: CornerCount(8)
	, NearPlaneIndex(0)
	, FarPlaneIndex(1)
	, LeftPlaneIndex(2)
	, RightPlaneIndex(3)
	, TopPlaneIndex(4)
	, BottomPlaneIndex(5)
	, NumPlanes(6)
{
	planes = new Plane[NumPlanes];
	cornerArray.resize(CornerCount);

	D3DXMatrixIdentity(&matrix);
}

BoundingFrustum::BoundingFrustum(D3DXMATRIX value)
	: CornerCount(8)
	, NearPlaneIndex(0)
	, FarPlaneIndex(1)
	, LeftPlaneIndex(2)
	, RightPlaneIndex(3)
	, TopPlaneIndex(4)
	, BottomPlaneIndex(5)
	, NumPlanes(6)
{
	planes = new Plane[NumPlanes];
	cornerArray.resize(CornerCount);

	SetMatrix(value);
}

BoundingFrustum::~BoundingFrustum()
{
	SAFE_DELETE_ARRAY(planes);
}

void BoundingFrustum::SetMatrix(D3DXMATRIX& value)
{
	matrix = value;
	planes[2].Normal.x = -value._14 - value._11;
	planes[2].Normal.y = -value._24 - value._21;
	planes[2].Normal.z = -value._34 - value._31;
	planes[2].D = -value._44 - value._41;
	planes[3].Normal.x = -value._14 + value._11;
	planes[3].Normal.y = -value._24 + value._21;
	planes[3].Normal.z = -value._34 + value._31;
	planes[3].D = -value._44 + value._41;
	planes[4].Normal.x = -value._14 + value._12;
	planes[4].Normal.y = -value._24 + value._22;
	planes[4].Normal.z = -value._34 + value._32;
	planes[4].D = -value._44 + value._42;
	planes[5].Normal.x = -value._14 - value._12;
	planes[5].Normal.y = -value._24 - value._22;
	planes[5].Normal.z = -value._34 - value._32;
	planes[5].D = -value._44 - value._42;
	planes[0].Normal.x = -value._13;
	planes[0].Normal.y = -value._23;
	planes[0].Normal.z = -value._33;
	planes[0].D = -value._43;
	planes[1].Normal.x = -value._14 + value._13;
	planes[1].Normal.y = -value._24 + value._23;
	planes[1].Normal.z = -value._34 + value._33;
	planes[1].D = -value._44 + value._43;
	for (int i = 0; i < 6; i++)
	{
		float single = Math::Length(planes[i].Normal);
		planes[i].Normal /= single;
		planes[i].D /= single;
	}

	Ray ray = ComputeIntersectionLine(planes[0], planes[2]);
	this->cornerArray[0] = ComputeIntersection(planes[4], ray);
	this->cornerArray[3] = ComputeIntersection(planes[5], ray);
	ray = ComputeIntersectionLine(planes[3], planes[0]);
	this->cornerArray[1] = ComputeIntersection(planes[4], ray);
	this->cornerArray[2] = ComputeIntersection(planes[5], ray);
	ray = ComputeIntersectionLine(planes[2], planes[1]);
	this->cornerArray[4] = ComputeIntersection(planes[4], ray);
	this->cornerArray[7] = ComputeIntersection(planes[5], ray);
	ray = ComputeIntersectionLine(planes[1], planes[3]);
	this->cornerArray[5] = ComputeIntersection(planes[4], ray);
	this->cornerArray[6] = ComputeIntersection(planes[5], ray);
}

ContainmentType BoundingFrustum::Contains(const D3DXVECTOR3& point)
{
	Plane* planeArray = planes;
	for (int i = 0; i < NumPlanes; i++)
	{
		Plane plane = planes[i];
		if (plane.Normal.x * point.x + plane.Normal.y * point.y + plane.Normal.z * point.z + plane.D > 1E-05f)
		{
			return ContainmentType::Disjoint;
		}
	}
	return ContainmentType::Contains;
}

ContainmentType BoundingFrustum::Contains(const BoundingBox* box)
{
	bool flag = false;
	for (int i = 0; i < NumPlanes; i++)
	{
		PlaneIntersectionType planeIntersectionType = planes[i].Intersects(box);
		if (planeIntersectionType == PlaneIntersectionType::Front)
		{
			return ContainmentType::Disjoint;
		}
		if (planeIntersectionType == PlaneIntersectionType::Intersecting)
		{
			flag = true;
		}
	}
	if (!flag)
	{
		return ContainmentType::Contains;
	}
	return ContainmentType::Intersects;
}

#if 0
ContainmentType BoundingFrustum::Contains(const BoundingFrustum* frustum)
{
	ContainmentType containmentType = ContainmentType::Disjoint;
	if (this.Intersects(frustum))
	{
		containmentType = ContainmentType::Contains;
		int num = 0;
		while (num < (int)this.cornerArray.Length)
		{
			if (this.Contains(frustum.cornerArray[num]) != ContainmentType::Disjoint)
			{
				num++;
			}
			else
			{
				containmentType = ContainmentType::Intersects;
				break;
			}
		}
	}
	return containmentType;
}

bool BoundingFrustum::Intersects(BoundingFrustum* frustum)
{
	D3DXVECTOR3 closestPoint;
	D3DXVECTOR3 vector3;
	D3DXVECTOR3 vector31;
	D3DXVECTOR3 vector32;
	D3DXVECTOR3 x;

	if (this.gjk == null)
	{
		this.gjk = new Gjk();
	}
	this.gjk.Reset();
	D3DXVECTOR3.Subtract(ref this.cornerArray[0], ref frustum.cornerArray[0], out closestPoint);
	if (closestPoint.LengthSquared() < 1E-05f)
	{
		D3DXVECTOR3.Subtract(ref this.cornerArray[0], ref frustum.cornerArray[1], out closestPoint);
	}
	float single = float.MaxValue;
	float maxLengthSquared = 0f;
	do
	{
		x.x = -closestPoint.x;
		x.y = -closestPoint.y;
		x.z = -closestPoint.z;
		this.SupportMapping(ref x, out vector32);
		frustum.SupportMapping(ref closestPoint, out vector31);
		D3DXVECTOR3.Subtract(ref vector32, ref vector31, out vector3);
		if (closestPoint.x * vector3.x + closestPoint.y * vector3.y + closestPoint.z * vector3.z > 0f)
		{
			return false;
		}
		this.gjk.AddSupportPoint(ref vector3);
		closestPoint = this.gjk.ClosestPoint;
		float single1 = single;
		single = closestPoint.LengthSquared();
		maxLengthSquared = 4E-05f * this.gjk.MaxLengthSquared;
		if (single1 - single > 1E-05f * single1)
		{
			continue;
		}
		return false;
	} while (!this.gjk.FullSimplex && single >= maxLengthSquared);
	return true;
}
#endif

ContainmentType BoundingFrustum::Contains(const BoundingSphere* sphere)
{
	D3DXVECTOR3 center = sphere->Center;
	float radius = sphere->Radius;
	int num = 0;
	for (int i = 0; i < NumPlanes; i++)
	{
		Plane plane = planes[i];
		float x = plane.Normal.x * center.x + plane.Normal.y * center.y + plane.Normal.z * center.z;
		float d = x + plane.D;
		if (d > radius)
		{
			return ContainmentType::Disjoint;
		}
		if (d < -radius)
		{
			num++;
		}
	}
	if (num != 6)
	{
		return ContainmentType::Intersects;
	}
	return ContainmentType::Contains;
}


void BoundingFrustum::Modify(RECT source, Viewport * viewport, D3DXMATRIX projection, D3DXMATRIX view)
{
	// Point in screen space of the center of the region selected
	D3DXVECTOR2 regionCenterScreen;
	regionCenterScreen.x = float(source.right + source.left) * 0.5f;
	regionCenterScreen.y = float(source.bottom + source.top) * 0.5f;

	// Generate the projection matrix for the screen region
	D3DXMATRIX regionProjMatrix = projection;

	float sourceWidth = float(source.right - source.left);
	float sourceHeight = float(source.bottom - source.top);
	float viewWidth = viewport->GetWidth();
	float viewHeight = viewport->GetHeight();

	// Calculate the region dimensions in the projection matrix. M11 is inverse of width, M22 is inverse of height.
	regionProjMatrix._11 /= (sourceWidth / viewWidth);
	regionProjMatrix._22 /= (sourceHeight / viewHeight);

	// Calculate the region center in the projection matrix. M31 is horizonatal center.
	regionProjMatrix._31 = -(regionCenterScreen.x - (viewWidth / 2.0f)) / (sourceWidth / 2.0f);

	// M32 is vertical center. Notice that the screen has low Y on top, projection has low Y on bottom.
	regionProjMatrix._32 = (regionCenterScreen.y - (viewHeight / 2.0f)) / (sourceHeight / 2.0f);

	SetMatrix(view * regionProjMatrix);
}


#if 0


/// <summary>Checks whether the current BoundingFrustum contains the specified BoundingBox.</summary>
/// <param name="box">The BoundingBox to test for overlap.</param>
/// <param name="result">[OutAttribute] Enumeration indicating the extent of overlap.</param>

/// <summary>Checks whether the current BoundingFrustum contains the specified BoundingFrustum.</summary>
/// <param name="frustum">The BoundingFrustum to check against the current BoundingFrustum.</param>


/// <summary>Checks whether the current BoundingFrustum contains the specified BoundingSphere.</summary>
/// <param name="sphere">The BoundingSphere to check against the current BoundingFrustum.</param>


/// <summary>Checks whether the current BoundingFrustum contains the specified BoundingSphere.</summary>
/// <param name="sphere">The BoundingSphere to test for overlap.</param>
/// <param name="result">[OutAttribute] Enumeration indicating the extent of overlap.</param>
public void Contains(ref BoundingSphere sphere, out ContainmentType result)
{
	D3DXVECTOR3 center = sphere.Center;
	float radius = sphere.Radius;
	int num = 0;
	Plane[] planeArray = this.planes;
	int num1 = 0;
	while (true)
	{
		if (num1 < (int)planeArray.Length)
		{
			Plane plane = planeArray[num1];
			float x = plane.Normal.x * center.x + plane.Normal.y * center.y + plane.Normal.z * center.z;
			float d = x + plane.D;
			if (d <= radius)
			{
				if (d < -radius)
				{
					num++;
				}
				num1++;
			}
			else
			{
				result = ContainmentType::Disjoint;
				break;
			}
		}
		else
		{
			result = (num == 6 ? ContainmentType::Contains : ContainmentType::Intersects);
			break;
		}
	}
}
#endif

D3DXVECTOR3 BoundingFrustum::ComputeIntersection(Plane plane, Ray ray)
{
	float d = (-plane.D - D3DXVec3Dot(&plane.Normal, &ray.Position)) / D3DXVec3Dot(&plane.Normal, &ray.Direction);
	return ray.Position + (ray.Direction * d);
}

Ray BoundingFrustum::ComputeIntersectionLine(Plane p1, Plane p2)
{
	Ray ray;
	D3DXVec3Cross(&ray.Direction, &p1.Normal, &p2.Normal);

	D3DXVec3Cross(&ray.Position, &((-p1.D * p2.Normal) + (p2.D * p1.Normal)), &ray.Direction);
	ray.Position /= Math::LengthSquared(ray.Direction);

	return ray;
}

