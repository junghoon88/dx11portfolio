#include "stdafx.h"
#include "BoundingSphere.h"

#include "../../Units/GameUnit.h"

BoundingSphere::BoundingSphere()
	: Center(0, 0, 0), Radius(0.5f)
	, sphereResolution(30)
	, myUnit(NULL)
	, socketNum(-1)
{
	assert(Radius > 0.0f);

	InitVertices();
	D3DXMatrixIdentity(&socketTransform);
	D3DXMatrixIdentity(&myWorld);
}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::Update(void)
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, transform.Scale.x, transform.Scale.y, transform.Scale.z);

	D3DXVECTOR3 rot = Math::ToRadian(transform.RotationDeg);
	D3DXMatrixRotationYawPitchRoll(&R, rot.y, rot.x, rot.z);
	D3DXMatrixTranslation(&T, transform.Position.x, transform.Position.y, transform.Position.z);

	myWorld = S * R * T;
	if (socketNum != -1)
	{
		D3DXMATRIX unitWorld = myUnit->Transformed();
		//myUnit->GetWorld(unitWorld);
		D3DXMATRIX socketTransform = myUnit->GetModel()->GetBone(socketNum)->GetTransform();

		myWorld = myWorld * socketTransform * unitWorld;
	}
}

void BoundingSphere::PostRender(void)
{

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
