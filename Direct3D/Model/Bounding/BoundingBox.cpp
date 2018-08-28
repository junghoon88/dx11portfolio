#include "stdafx.h"
#include "BoundingBox.h"


BoundingBox::BoundingBox(GameModel* mymodel, D3DXVECTOR3 min, D3DXVECTOR3 max)
	: Bounding(mymodel)
	, CornerCount(8), Min(min), Max(max)
{
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::Update(void)
{
	Bounding::Update();

	if (bShow)
	{
		gModelShape->AddBoundingBox(this, color);
	}
}

void BoundingBox::PostRender(void)
{
	Bounding::PostRender();

	ImGui::DragFloat3("Min", Min, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat3("Max", Max, 0.1f, -1000.0f, 1000.0f);
}

void BoundingBox::GetCorners(vector<D3DXVECTOR3>& vec)
{
	vector<D3DXVECTOR3> corners;
	corners.push_back(D3DXVECTOR3(Min.x, Max.y, Max.z));
	corners.push_back(D3DXVECTOR3(Max.x, Max.y, Max.z));
	corners.push_back(D3DXVECTOR3(Max.x, Min.y, Max.z));
	corners.push_back(D3DXVECTOR3(Min.x, Min.y, Max.z));
	corners.push_back(D3DXVECTOR3(Min.x, Max.y, Min.z));
	corners.push_back(D3DXVECTOR3(Max.x, Max.y, Min.z));
	corners.push_back(D3DXVECTOR3(Max.x, Min.y, Min.z));
	corners.push_back(D3DXVECTOR3(Min.x, Min.y, Min.z));

	size_t count = corners.size();
	vec.resize(count);
	for (size_t i = 0; i < count; i++)
	{
		D3DXVec3TransformCoord(&vec[i], &corners[i], &myWorld);
	}
}

D3DXVECTOR3 BoundingBox::GetMin(void) const
{
	D3DXVECTOR3 min = D3DXVECTOR3(Min.x, Min.y, Min.z);
	D3DXVec3TransformCoord(&min, &min, &myWorld);
	return min;
}

D3DXVECTOR3 BoundingBox::GetMax(void) const
{
	D3DXVECTOR3 max = D3DXVECTOR3(Max.x, Max.y, Max.z);
	D3DXVec3TransformCoord(&max, &max, &myWorld);
	return max;
}





#if 0


{
	/// <summary>Defines an axis-aligned box-shaped 3D volume.</summary>
	[Serializable]
[TypeConverter(typeof(BoundingBoxConverter))]
public class BoundingBox : IEquatable<BoundingBox>
{
	
	/// <summary>Creates the smallest BoundingBox that will contain a group of points.</summary>
	/// <param name="points">A list of points the BoundingBox should contain.</param>
	public static BoundingBox CreateFromPoints(IEnumerable<D3DXVECTOR3> points)
	{
		if (points == null)
		{
			throw new ArgumentNullException();
		}
		bool flag = false;
		D3DXVECTOR3 vector3 = new D3DXVECTOR3(float.MaxValue);
		D3DXVECTOR3 vector31 = new D3DXVECTOR3(float.MinValue);
		foreach(D3DXVECTOR3 point in points)
		{
			D3DXVECTOR3 vector32 = point;
			D3DXVECTOR3.Min(ref vector3, ref vector32, out vector3);
			D3DXVECTOR3.Max(ref vector31, ref vector32, out vector31);
			flag = true;
		}
		if (!flag)
		{
			throw new ArgumentException(FrameworkResources.BoundingBoxZeroPoints);
		}
		return new BoundingBox(vector3, vector31);
	}

	/// <summary>Creates the smallest BoundingBox that will contain the specified BoundingSphere.</summary>
	/// <param name="sphere">The BoundingSphere to contain.</param>
	public static BoundingBox CreateFromSphere(BoundingSphere sphere)
	{
		BoundingBox x = new BoundingBox();
		x.Min.x = sphere.Center.x - sphere.Radius;
		x.Min.y = sphere.Center.y - sphere.Radius;
		x.Min.z = sphere.Center.z - sphere.Radius;
		x.Max.x = sphere.Center.x + sphere.Radius;
		x.Max.y = sphere.Center.y + sphere.Radius;
		x.Max.z = sphere.Center.z + sphere.Radius;
		return x;
	}

	/// <summary>Creates the smallest BoundingBox that will contain the specified BoundingSphere.</summary>
	/// <param name="sphere">The BoundingSphere to contain.</param>
	/// <param name="result">[OutAttribute] The created BoundingBox.</param>
	public static void CreateFromSphere(ref BoundingSphere sphere, out BoundingBox result)
	{
		result = new BoundingBox();
		result.Min.x = sphere.Center.x - sphere.Radius;
		result.Min.y = sphere.Center.y - sphere.Radius;
		result.Min.z = sphere.Center.z - sphere.Radius;
		result.Max.x = sphere.Center.x + sphere.Radius;
		result.Max.y = sphere.Center.y + sphere.Radius;
		result.Max.z = sphere.Center.z + sphere.Radius;
	}

	/// <summary>Creates the smallest BoundingBox that contains the two specified BoundingBox instances.</summary>
	/// <param name="original">One of the BoundingBoxs to contain.</param>
	/// <param name="additional">One of the BoundingBoxs to contain.</param>
	public static BoundingBox CreateMerged(BoundingBox original, BoundingBox additional)
	{
		BoundingBox boundingBox = new BoundingBox();
		D3DXVECTOR3.Min(ref original.Min, ref additional.Min, out boundingBox.Min);
		D3DXVECTOR3.Max(ref original.Max, ref additional.Max, out boundingBox.Max);
		return boundingBox;
	}

	/// <summary>Creates the smallest BoundingBox that contains the two specified BoundingBox instances.</summary>
	/// <param name="original">One of the BoundingBox instances to contain.</param>
	/// <param name="additional">One of the BoundingBox instances to contain.</param>
	/// <param name="result">[OutAttribute] The created BoundingBox.</param>
	public static void CreateMerged(ref BoundingBox original, ref BoundingBox additional, out BoundingBox result)
	{
		result = new BoundingBox();
		D3DXVECTOR3 vector3;
		D3DXVECTOR3 vector31;
		D3DXVECTOR3.Min(ref original.Min, ref additional.Min, out vector31);
		D3DXVECTOR3.Max(ref original.Max, ref additional.Max, out vector3);
		result.Min = vector31;
		result.Max = vector3;
	}

	/// <summary>Determines whether two instances of BoundingBox are equal.</summary>
	/// <param name="other">The BoundingBox to compare with the current BoundingBox.</param>
	public bool Equals(BoundingBox other)
	{
		if (Min != other.Min)
		{
			return false;
		}
		return Max == other.Max;
	}

	/// <summary>Determines whether two instances of BoundingBox are equal.</summary>
	/// <param name="obj">The Object to compare with the current BoundingBox.</param>
	public override bool Equals(object obj)
	{
		bool flag = false;
		if (obj is BoundingBox)
		{
			flag = Equals((BoundingBox)obj);
		}
		return flag;
	}

	/// <summary>Gets an array of points that make up the corners of the BoundingBox.</summary>
	public D3DXVECTOR3[] GetCorners()
	{
		D3DXVECTOR3[] vector3 = new D3DXVECTOR3[]{ new D3DXVECTOR3(Min.x, Max.y, Max.z), new D3DXVECTOR3(Max.x, Max.y, Max.z), new D3DXVECTOR3(Max.x, Min.y, Max.z), new D3DXVECTOR3(Min.x, Min.y, Max.z), new D3DXVECTOR3(Min.x, Max.y, Min.z), new D3DXVECTOR3(Max.x, Max.y, Min.z), new D3DXVECTOR3(Max.x, Min.y, Min.z), new D3DXVECTOR3(Min.x, Min.y, Min.z) };
		return vector3;
	}

	/// <summary>Gets the array of points that make up the corners of the BoundingBox.</summary>
	/// <param name="corners">An existing array of at least 8 D3DXVECTOR3 points where the corners of the BoundingBox are written.</param>
	public void GetCorners(D3DXVECTOR3[] corners)
	{
		if (corners == null)
		{
			throw new ArgumentNullException("corners");
		}
		if ((int)corners.Length < 8)
		{
			throw new ArgumentOutOfRangeException("corners", FrameworkResources.NotEnoughCorners);
		}
		corners[0].x = Min.x;
		corners[0].y = Max.y;
		corners[0].z = Max.z;
		corners[1].x = Max.x;
		corners[1].y = Max.y;
		corners[1].z = Max.z;
		corners[2].x = Max.x;
		corners[2].y = Min.y;
		corners[2].z = Max.z;
		corners[3].x = Min.x;
		corners[3].y = Min.y;
		corners[3].z = Max.z;
		corners[4].x = Min.x;
		corners[4].y = Max.y;
		corners[4].z = Min.z;
		corners[5].x = Max.x;
		corners[5].y = Max.y;
		corners[5].z = Min.z;
		corners[6].x = Max.x;
		corners[6].y = Min.y;
		corners[6].z = Min.z;
		corners[7].x = Min.x;
		corners[7].y = Min.y;
		corners[7].z = Min.z;
	}

	/// <summary>Gets the hash code for this instance.</summary>
	public override int GetHashCode()
	{
		return Min.GetHashCode() + Max.GetHashCode();
	}

	/// <summary>Checks whether the current BoundingBox intersects another BoundingBox.</summary>
	/// <param name="box">The BoundingBox to check for intersection with.</param>
	public bool Intersects(BoundingBox box)
	{
		if (Max.x < box.Min.x || Min.x > box.Max.x)
		{
			return false;
		}
		if (Max.y < box.Min.y || Min.y > box.Max.y)
		{
			return false;
		}
		if (Max.z >= box.Min.z && Min.z <= box.Max.z)
		{
			return true;
		}
		return false;
	}

	/// <summary>Checks whether the current BoundingBox intersects another BoundingBox.</summary>
	/// <param name="box">The BoundingBox to check for intersection with.</param>
	/// <param name="result">[OutAttribute] true if the BoundingBox instances intersect; false otherwise.</param>
	public void Intersects(ref BoundingBox box, out bool result)
	{
		result = false;
		if (Max.x < box.Min.x || Min.x > box.Max.x)
		{
			return;
		}
		if (Max.y < box.Min.y || Min.y > box.Max.y)
		{
			return;
		}
		if (Max.z < box.Min.z || Min.z > box.Max.z)
		{
			return;
		}
		result = true;
	}

	/// <summary>Checks whether the current BoundingBox intersects a BoundingFrustum.</summary>
	/// <param name="frustum">The BoundingFrustum to check for intersection with.</param>
	public bool Intersects(BoundingFrustum frustum)
	{
		if (null == frustum)
		{
			throw new ArgumentNullException("frustum", FrameworkResources.NullNotAllowed);
		}
		return frustum.Intersects(this);
	}

	/// <summary>Checks whether the current BoundingBox intersects a Plane.</summary>
	/// <param name="plane">The Plane to check for intersection with.</param>
	public PlaneIntersectionType Intersects(Plane plane)
	{
		D3DXVECTOR3 vector3 = new D3DXVECTOR3();
		D3DXVECTOR3 vector31 = new D3DXVECTOR3();
		vector31.x = (plane.Normal.x >= 0f ? Min.x : Max.x);
		vector31.y = (plane.Normal.y >= 0f ? Min.y : Max.y);
		vector31.z = (plane.Normal.z >= 0f ? Min.z : Max.z);
		vector3.x = (plane.Normal.x >= 0f ? Max.x : Min.x);
		vector3.y = (plane.Normal.y >= 0f ? Max.y : Min.y);
		vector3.z = (plane.Normal.z >= 0f ? Max.z : Min.z);
		if (plane.Normal.x * vector31.x + plane.Normal.y * vector31.y + plane.Normal.z * vector31.z + plane.D > 0f)
		{
			return PlaneIntersectionType.Front;
		}
		if (plane.Normal.x * vector3.x + plane.Normal.y * vector3.y + plane.Normal.z * vector3.z + plane.D < 0f)
		{
			return PlaneIntersectionType.Back;
		}
		return PlaneIntersectionType.Intersecting;
	}

	/// <summary>Checks whether the current BoundingBox intersects a Plane.</summary>
	/// <param name="plane">The Plane to check for intersection with.</param>
	/// <param name="result">[OutAttribute] An enumeration indicating whether the BoundingBox intersects the Plane.</param>
	public void Intersects(ref Plane plane, out PlaneIntersectionType result)
	{
		D3DXVECTOR3 vector3 = new D3DXVECTOR3();
		D3DXVECTOR3 vector31 = new D3DXVECTOR3();
		vector31.x = (plane.Normal.x >= 0f ? Min.x : Max.x);
		vector31.y = (plane.Normal.y >= 0f ? Min.y : Max.y);
		vector31.z = (plane.Normal.z >= 0f ? Min.z : Max.z);
		vector3.x = (plane.Normal.x >= 0f ? Max.x : Min.x);
		vector3.y = (plane.Normal.y >= 0f ? Max.y : Min.y);
		vector3.z = (plane.Normal.z >= 0f ? Max.z : Min.z);
		if (plane.Normal.x * vector31.x + plane.Normal.y * vector31.y + plane.Normal.z * vector31.z + plane.D > 0f)
		{
			result = PlaneIntersectionType.Front;
			return;
		}
		if (plane.Normal.x * vector3.x + plane.Normal.y * vector3.y + plane.Normal.z * vector3.z + plane.D < 0f)
		{
			result = PlaneIntersectionType.Back;
			return;
		}
		result = PlaneIntersectionType.Intersecting;
	}

	/// <summary>Checks whether the current BoundingBox intersects a Ray.</summary>
	/// <param name="ray">The Ray to check for intersection with.</param>
	public float ? Intersects(Ray ray)
	{
		float single = 0f;
		float single1 = float.MaxValue;
		if (Math.Abs(ray.Direction.x) >= 1E-06f)
		{
			float x = 1f / ray.Direction.x;
			float x1 = (Min.x - ray.Position.x) * x;
			float x2 = (Max.x - ray.Position.x) * x;
			if (x1 > x2)
			{
				float single2 = x1;
				x1 = x2;
				x2 = single2;
			}
			single = MathHelper.Max(x1, single);
			single1 = MathHelper.Min(x2, single1);
			if (single > single1)
			{
				return null;
			}
		}
		else if (ray.Position.x < Min.x || ray.Position.x > Max.x)
		{
			return null;
		}
		if (Math.Abs(ray.Direction.y) >= 1E-06f)
		{
			float y = 1f / ray.Direction.y;
			float y1 = (Min.y - ray.Position.y) * y;
			float y2 = (Max.y - ray.Position.y) * y;
			if (y1 > y2)
			{
				float single3 = y1;
				y1 = y2;
				y2 = single3;
			}
			single = MathHelper.Max(y1, single);
			single1 = MathHelper.Min(y2, single1);
			if (single > single1)
			{
				return null;
			}
		}
		else if (ray.Position.y < Min.y || ray.Position.y > Max.y)
		{
			return null;
		}
		if (Math.Abs(ray.Direction.z) >= 1E-06f)
		{
			float z = 1f / ray.Direction.z;
			float z1 = (Min.z - ray.Position.z) * z;
			float z2 = (Max.z - ray.Position.z) * z;
			if (z1 > z2)
			{
				float single4 = z1;
				z1 = z2;
				z2 = single4;
			}
			single = MathHelper.Max(z1, single);
			single1 = MathHelper.Min(z2, single1);
			if (single > single1)
			{
				return null;
			}
		}
		else if (ray.Position.z < Min.z || ray.Position.z > Max.z)
		{
			return null;
		}
		return new float ? (single);
	}

	/// <summary>Checks whether the current BoundingBox intersects a Ray.</summary>
	/// <param name="ray">The Ray to check for intersection with.</param>
	/// <param name="result">[OutAttribute] Distance at which the ray intersects the BoundingBox, or null if there is no intersection.</param>
	public void Intersects(ref Ray ray, out float ? result)
	{
		result = null;
		float single = 0f;
		float single1 = float.MaxValue;
		if (Math.Abs(ray.Direction.x) >= 1E-06f)
		{
			float x = 1f / ray.Direction.x;
			float x1 = (Min.x - ray.Position.x) * x;
			float x2 = (Max.x - ray.Position.x) * x;
			if (x1 > x2)
			{
				float single2 = x1;
				x1 = x2;
				x2 = single2;
			}
			single = MathHelper.Max(x1, single);
			single1 = MathHelper.Min(x2, single1);
			if (single > single1)
			{
				return;
			}
		}
		else if (ray.Position.x < Min.x || ray.Position.x > Max.x)
		{
			return;
		}
		if (Math.Abs(ray.Direction.y) >= 1E-06f)
		{
			float y = 1f / ray.Direction.y;
			float y1 = (Min.y - ray.Position.y) * y;
			float y2 = (Max.y - ray.Position.y) * y;
			if (y1 > y2)
			{
				float single3 = y1;
				y1 = y2;
				y2 = single3;
			}
			single = MathHelper.Max(y1, single);
			single1 = MathHelper.Min(y2, single1);
			if (single > single1)
			{
				return;
			}
		}
		else if (ray.Position.y < Min.y || ray.Position.y > Max.y)
		{
			return;
		}
		if (Math.Abs(ray.Direction.z) >= 1E-06f)
		{
			float z = 1f / ray.Direction.z;
			float z1 = (Min.z - ray.Position.z) * z;
			float z2 = (Max.z - ray.Position.z) * z;
			if (z1 > z2)
			{
				float single4 = z1;
				z1 = z2;
				z2 = single4;
			}
			single = MathHelper.Max(z1, single);
			single1 = MathHelper.Min(z2, single1);
			if (single > single1)
			{
				return;
			}
		}
		else if (ray.Position.z < Min.z || ray.Position.z > Max.z)
		{
			return;
		}
		result = new float ? (single);
	}

	/// <summary>Checks whether the current BoundingBox intersects a BoundingSphere.</summary>
	/// <param name="sphere">The BoundingSphere to check for intersection with.</param>
	public bool Intersects(BoundingSphere sphere)
	{
		float single;
		D3DXVECTOR3 vector3;
		D3DXVECTOR3.Clamp(ref sphere.Center, ref Min, ref Max, out vector3);
		D3DXVECTOR3.DistanceSquared(ref sphere.Center, ref vector3, out single);
		if (single <= sphere.Radius * sphere.Radius)
		{
			return true;
		}
		return false;
	}

	/// <summary>Checks whether the current BoundingBox intersects a BoundingSphere.</summary>
	/// <param name="sphere">The BoundingSphere to check for intersection with.</param>
	/// <param name="result">[OutAttribute] true if the BoundingBox and BoundingSphere intersect; false otherwise.</param>
	public void Intersects(ref BoundingSphere sphere, out bool result)
	{
		float single;
		D3DXVECTOR3 vector3;
		D3DXVECTOR3.Clamp(ref sphere.Center, ref Min, ref Max, out vector3);
		D3DXVECTOR3.DistanceSquared(ref sphere.Center, ref vector3, out single);
		result = (single > sphere.Radius * sphere.Radius ? false : true);
	}

	/// <summary>Determines whether two instances of BoundingBox are equal.</summary>
	/// <param name="a">BoundingBox to compare.</param>
	/// <param name="b">BoundingBox to compare.</param>
	public static bool operator ==(BoundingBox a, BoundingBox b)
	{
		return a.Equals(b);
	}

	/// <summary>Determines whether two instances of BoundingBox are not equal.</summary>
	/// <param name="a">The object to the left of the inequality operator.</param>
	/// <param name="b">The object to the right of the inequality operator.</param>
	public static bool operator !=(BoundingBox a, BoundingBox b)
	{
		if (a.Min != b.Min)
		{
			return true;
		}
		return a.Max != b.Max;
	}

	internal void SupportMapping(ref D3DXVECTOR3 v, out D3DXVECTOR3 result)
	{
		result = new D3DXVECTOR3();
		result.x = (v.x >= 0f ? Max.x : Min.x);
		result.y = (v.y >= 0f ? Max.y : Min.y);
		result.z = (v.z >= 0f ? Max.z : Min.z);
	}

	/// <summary>Returns a String that represents the current BoundingBox.</summary>
	public override string ToString()
	{
		CultureInfo currentCulture = CultureInfo.CurrentCulture;
		object[] str = new object[]{ Min.ToString(), Max.ToString() };
		return string.Format(currentCulture, "{{Min:{0} Max:{1}}}", str);
	}
}
}

#endif