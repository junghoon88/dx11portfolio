#include "stdafx.h"
#include "Ray.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "BoundingFrustum.h"

#include "Plane.h"

Ray Ray::CalculateCursorRay(ExecuteValues* values)
{
	D3DXVECTOR3 start;
	D3DXVECTOR3 direction;
	values->MainCamera->GetPosition(&start);
	direction = values->MainCamera->GetDirection(values->Viewport, values->Perspective);

	return Ray(start, direction);
}

bool Ray::Intersects(BoundingBox* box, OUT float* result)
{
	float single = 0.0f;
	float single1 = Math::FloatMaxValue;

	D3DXVECTOR3 boxMin = box->GetOrgMin();
	D3DXVECTOR3 boxMax = box->GetOrgMax();

	D3DXMATRIX mat = box->GetSocketTransform();
	D3DXMatrixInverse(&mat, NULL, &mat);

	D3DXVECTOR3 pos, dir;
	D3DXVec3TransformCoord(&pos, &Position, &mat);
	D3DXVec3TransformNormal(&dir, &Direction, &mat);

	//dir = dir - pos;

	//방향벡터에 X성분이 0이 아니면
	if (abs(dir.x) >= 1E-06f)
	{
		float x1 = (boxMin.x - pos.x) / dir.x;
		float x2 = (boxMax.x - pos.x) / dir.x;

		Math::SwapMinMax(&x1, &x2);

		single  = Math::Max(x1, single);
		single1 = Math::Min(x2, single1);

		if (single > single1)
		{
			return false;
		}
	}
	else if (pos.x < boxMin.x || pos.x > boxMax.x)
	{
		//2D 사각형 min, max 체크
		return false;
	}

	//방향벡터에 Y성분이 0이 아니면
	if (abs(dir.y) >= 1E-06f)
	{
		float y1 = (boxMin.y - pos.y) / dir.y;
		float y2 = (boxMax.y - pos.y) / dir.y;

		Math::SwapMinMax(&y1, &y2);

		single  = Math::Max(y1, single);
		single1 = Math::Min(y2, single1);
		if (single > single1)
		{
			return false;
		}
	}
	else if (pos.y < boxMin.y || pos.y > boxMax.y)
	{
		//2D 사각형 min, max 체크
		return false;
	}

	//방향벡터에 Z성분이 0이 아니면
	if (abs(dir.z) >= 1E-06f)
	{
		float z1 = (boxMin.z - pos.z) / dir.z;
		float z2 = (boxMax.z - pos.z) / dir.z;

		Math::SwapMinMax(&z1, &z2);

		single  = Math::Max(z1, single);
		single1 = Math::Min(z2, single1);
		if (single > single1)
		{
			return false;
		}
	}
	else if (pos.z < boxMin.z || pos.z > boxMax.z)
	{
		//2D 사각형 min, max 체크
		return false;
	}

	//X, Y, Z 축 계산해서 Max값..?

	*result = single;
	return true;
}

bool Ray::Intersects(BoundingSphere* sphere, OUT float* result)
{
	float x = sphere->Center.x - Position.x;
	float y = sphere->Center.y - Position.y;
	float z = sphere->Center.z - Position.z;
	float single = x * x + y * y + z * z;
	float radius = sphere->Radius * sphere->Radius;

	if (single <= radius)
	{
		//Ray Position이 Sphere 내부에 있음
		*result = 0.0f;
		return true;
	}
	
	float x1 = x * Direction.x + y * Direction.y + z * Direction.z;
	if (x1 < 0.0f)
	{
		//내적(dot)결과가 음수일때 -> 겹칠일 없음
		return false;
	}

	float single1 = single - x1 * x1;
	if (single1 > radius)
	{
		//sphere 반경에 들어가지 않는다.
		return false;
	}

	float single2 = (float)sqrt((double)(radius - single1));
	*result = (x1 - single2);

	return true;
}

bool Ray::Intersects(BoundingFrustum* frustum, OUT float* result)
{
	float d;
	float single;
	ContainmentType containmentType = frustum->Contains(Position);
	if (containmentType == ContainmentType::Contains)
	{
		*result = 0.0f;
		return true;
	}

	float single1 = Math::FloatMinValue;
	float single2 = Math::FloatMaxValue;

	Plane* planeArray = frustum->planes;
	
	int num = 0;
	while (true)
	{
		if (num < frustum->NumPlanes)
		{
			Plane plane = planeArray[num];
			D3DXVECTOR3 normal = plane.Normal;

			single = D3DXVec3Dot(&Direction, &normal);
			d = D3DXVec3Dot(&Position, &normal);
			d += plane.D;
			if (abs(single) >= 1E-05f)
			{
				float single3 = -d / single;
				if (single < 0.0f)
				{
					if (single3 > single2)
					{
						break;
					}
					else if (single3 > single1)
					{
						single1 = single3;
					}
				}
				else if (single3 < single1)
				{
					break;
				}
				else if (single3 < single2)
				{
					single2 = single3;
				}
			}
			else if (d > 0.0f)
			{
				break;
			}
			num++;
		}
		else
		{
			float single4 = (single1 >= 0.0f ? single1 : single2);
			if (single4 < 0.0f)
			{
				break;
			}
			*result = single4;
			return true;
		}
	}
	return false;
}


#if 0
/// <summary>Checks whether the Ray intersects a specified BoundingFrustum.</summary>
/// <param name="frustum">The BoundingFrustum to check for intersection with the Ray.</param>
public float ? Intersects(BoundingFrustum frustum)
{
	if (frustum == null)
	{
		throw new ArgumentNullException("frustum");
	}
	return frustum.Intersects(this);
}

/// <summary>Determines whether this Ray intersects a specified Plane.</summary>
/// <param name="plane">The Plane with which to calculate this Ray's intersection.</param>
public float ? Intersects(Plane plane)
{
	float x = plane.Normal.x * this.Direction.x + plane.Normal.y * this.Direction.y + plane.Normal.z * this.Direction.z;
	if (abs(x) < 1E-05f)
	{
		return null;
	}
	float single = plane.Normal.x * this.Position.x + plane.Normal.y * this.Position.y + plane.Normal.z * this.Position.z;
	float d = (-plane.D - single) / x;
	if (d < 0f)
	{
		if (d < -1E-05f)
		{
			return null;
		}
		d = 0f;
	}
	return new float ? (d);
}

/// <summary>Determines whether this Ray intersects a specified Plane.</summary>
/// <param name="plane">The Plane with which to calculate this Ray's intersection.</param>
/// <param name="result">[OutAttribute] The distance at which this Ray intersects the specified Plane, or null if there is no intersection.</param>
public void Intersects(ref Plane plane, out float ? result)
{
	float x = plane.Normal.x * this.Direction.x + plane.Normal.y * this.Direction.y + plane.Normal.z * this.Direction.z;
	if (abs(x) < 1E-05f)
	{
		result = null;
		return;
	}
	float single = plane.Normal.x * this.Position.x + plane.Normal.y * this.Position.y + plane.Normal.z * this.Position.z;
	float d = (-plane.D - single) / x;
	if (d < 0f)
	{
		if (d < -1E-05f)
		{
			result = null;
			return;
		}
		result = new float ? (0f);
	}
	result = new float ? (d);
}

#endif