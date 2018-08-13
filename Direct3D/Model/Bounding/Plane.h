#pragma once

class BoundingBox;
class BoundingSphere;
class BoundingFrustum;

enum class PlaneIntersectionType
{
	/// <summary>There is no intersection, and the bounding volume is in the positive half-space of the Plane.</summary>
	Front,
	/// <summary>There is no intersection, and the bounding volume is in the negative half-space of the Plane.</summary>
	Back,
	/// <summary>The Plane is intersected.</summary>
	Intersecting
};

struct Plane
{
	D3DXVECTOR3 Normal;
	float D;

	Plane()
	{
		Normal = D3DXVECTOR3(0, 0, 0);
		D = 0.0f;
	}


	//
	PlaneIntersectionType Intersects(const BoundingBox* box);
	PlaneIntersectionType Intersects(const BoundingFrustum* plane);
	PlaneIntersectionType Intersects(const BoundingSphere* sphere);

};

