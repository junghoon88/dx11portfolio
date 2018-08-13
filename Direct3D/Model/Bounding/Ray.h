#pragma once

class BoundingBox;
class BoundingSphere;
class BoundingFrustum;

/// <summary>Indicates the extent to which bounding volumes intersect or contain one another.</summary>
enum class ContainmentType
{
	/// <summary>Indicates there is no overlap between the bounding volumes.</summary>
	Disjoint,
	/// <summary>Indicates that one bounding volume completely contains the other.</summary>
	Contains,
	/// <summary>Indicates that the bounding volumes partially overlap.</summary>
	Intersects
};

struct Ray
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Direction;

	Ray()
	{
		Position  = D3DXVECTOR3(0, 0, 0);
		Direction = D3DXVECTOR3(0, 0, 0);
	}

	Ray(D3DXVECTOR3 pos, D3DXVECTOR3 dir)
	{
		Position = pos;
		Direction = dir;
	}

	//static
	static Ray CalculateCursorRay(ExecuteValues* values);


	//public
	bool Intersects(BoundingBox* sphere, OUT float* result);
	bool Intersects(BoundingSphere* sphere, OUT float* result);
	bool Intersects(BoundingFrustum* frustum, OUT float* result);

	
};

