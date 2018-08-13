#pragma once

struct Ray;
struct Plane;
class BoundingBox;
class BoundingSphere;
enum class ContainmentType;

class BoundingFrustum
{
public:
	friend Ray;
	friend Plane;


public:
	BoundingFrustum();
	BoundingFrustum(D3DXMATRIX value);
	~BoundingFrustum();


	void SetMatrix(D3DXMATRIX& value);

	ContainmentType Contains(const D3DXVECTOR3& point);
	ContainmentType Contains(const BoundingBox* box);
	//ContainmentType Contains(const BoundingFrustum* frustum);
	ContainmentType Contains(const BoundingSphere* sphere);

	bool Intersects(BoundingFrustum* frustum);

	void Modify(RECT source, Viewport* viewport, D3DXMATRIX projection, D3DXMATRIX view);

private:
	D3DXVECTOR3 ComputeIntersection(Plane plane, Ray ray);
	Ray ComputeIntersectionLine(Plane p1, Plane p2);


public:
	inline vector<D3DXVECTOR3> GetCorners(void) { return cornerArray; }


public:
	const int CornerCount;

private:
	const int NearPlaneIndex;
	const int FarPlaneIndex;
	const int LeftPlaneIndex;
	const int RightPlaneIndex;
	const int TopPlaneIndex;
	const int BottomPlaneIndex;
	const int NumPlanes;

	D3DXMATRIX matrix;
	Plane*		planes;

	vector<D3DXVECTOR3> cornerArray;

#if 0

	Gjk gjk;
	/// <summary>Gets the bottom plane of the BoundingFrustum.</summary>
	public Plane Bottom
	{
		get
	{
		return this.planes[5];
	}
	}

		/// <summary>Gets the far plane of the BoundingFrustum.</summary>
		public Plane Far
	{
		get
	{
		return this.planes[1];
	}
	}

		/// <summary>Gets the left plane of the BoundingFrustum.</summary>
		public Plane Left
	{
		get
	{
		return this.planes[2];
	}
	}

		/// <summary>Gets or sets the D3DXMATRIX that describes this bounding frustum.</summary>
		public D3DXMATRIX D3DXMATRIX
	{
		get
	{
		return this.matrix;
	}
		set
	{
		this.SetMatrix(ref value);
	}
	}

		/// <summary>Gets the near plane of the BoundingFrustum.</summary>
		public Plane Near
	{
		get
	{
		return this.planes[0];
	}
	}

		/// <summary>Gets the right plane of the BoundingFrustum.</summary>
		public Plane Right
	{
		get
	{
		return this.planes[3];
	}
	}

		/// <summary>Gets the top plane of the BoundingFrustum.</summary>
		public Plane Top
	{
		get
	{
		return this.planes[4];
	}
	}
#endif
};

