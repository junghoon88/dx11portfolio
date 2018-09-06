#include "stdafx.h"
#include "BoundingSphere.h"

#include "../../Units/GameUnit.h"

BoundingSphere::BoundingSphere(GameModel* mymodel)
	: Bounding(mymodel)
	, Center(0, 0, 0), Radius(0.5f)
	, sphereResolution(30)
{
	assert(Radius > 0.0f);

	type = BoundingType::Sphere;

	InitVertices();
}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::Update(void)
{
	Bounding::Update();

	if (bShow)
	{
		gModelShape->AddBoundingSphere(this, color);
	}
}

void BoundingSphere::PostRender(void)
{
	Bounding::PostRender();
}

void BoundingSphere::GetCorners(vector<D3DXVECTOR3>& vec)
{
	vec.resize(unitSphere.size());

	for (size_t i = 0; i < unitSphere.size(); i++)
	{
		D3DXVECTOR3 v;
		v = unitSphere[i] * Radius + Center;
		D3DXVec3TransformCoord(&v, &v, &myWorld);

		vec[i] = v;
	}
}

void BoundingSphere::InitVertices(void)
{
	// Compute our step around each circle
	float step = Math::TwoPI / sphereResolution;

	// Create the loop on the XY plane first
	for (float a = 0.0f; a < Math::TwoPI; a += step)
	{
		unitSphere.push_back(D3DXVECTOR3((float)cosf(a), (float)sinf(a), 0.0f));
		unitSphere.push_back(D3DXVECTOR3((float)cosf(a + step), (float)sinf(a + step), 0.0f));
	}

	// Next on the XZ plane
	for (float a = 0.0f; a < Math::TwoPI; a += step)
	{
		unitSphere.push_back(D3DXVECTOR3((float)cosf(a), 0.0f, (float)sinf(a)));
		unitSphere.push_back(D3DXVECTOR3((float)cosf(a + step), 0.0f, (float)sinf(a + step)));
	}

	// Finally on the YZ plane
	for (float a = 0.0f; a < Math::TwoPI; a += step)
	{
		unitSphere.push_back(D3DXVECTOR3(0.0f, (float)cosf(a), (float)sinf(a)));
		unitSphere.push_back(D3DXVECTOR3(0.0f, (float)cosf(a + step), (float)sinf(a + step)));
	}

}
