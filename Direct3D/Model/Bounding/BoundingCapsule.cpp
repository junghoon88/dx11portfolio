#include "stdafx.h"
#include "BoundingCapsule.h"
#include "BoundingSphere.h"

#define SMALL_NUM (float)1e-6

BoundingCapsule::BoundingCapsule(GameModel* mymodel, D3DXVECTOR3 center, float radius, float height)
	: Bounding(mymodel)
	, center(center), radius(radius), height(height)
	, sphereResolution(36.0f)
{
	assert(radius > 0.0f);

	type = BoundingType::Capsule;

	InitVertices();
}

BoundingCapsule::~BoundingCapsule()
{
}


void BoundingCapsule::Update(void)
{
	Bounding::Update();

	if (bShow)
	{
		gModelShape->AddBoundingCapsule(this, color);
	}
}

void BoundingCapsule::PostRender(void)
{
	Bounding::PostRender();

	ImGui::InputFloat3("Center", center);
	ImGui::DragFloat("Radius", &radius, 0.1f);
	ImGui::DragFloat("Height", &height, 0.1f);

	if (ImGui::Button("Clac"))
	{
		CalcVertices();
	}
}

void BoundingCapsule::SetCenter(D3DXVECTOR3 vec)
{
	center = vec;
	CalcVertices();
}

void BoundingCapsule::SetRadius(float r)
{
	radius = r;
	CalcVertices();
}

void BoundingCapsule::SetHeight(float h)
{
	height = h;
	CalcVertices();
}

void BoundingCapsule::GetCorners(vector<D3DXVECTOR3>& vec)
{
	vec.resize(verticesCircleBase.size());

	for (size_t i = 0; i < verticesCircleBase.size(); i++)
	{
		D3DXVec3TransformCoord(&vec[i], &verticesCircleBase[i], &myWorld);
	}
}

void BoundingCapsule::InitVertices(void)
{
	D3DXVECTOR3 center1 = center + D3DXVECTOR3(0.0f,  0.5f, 0.0f) * height;
	D3DXVECTOR3 center2 = center + D3DXVECTOR3(0.0f, -0.5f, 0.0f) * height;
	float step = Math::TwoPI / (float)sphereResolution;

	//round 2개
	for (float a = 0.0f; a < Math::TwoPI; a += step)
	{
		verticesCircleBase.push_back(center1 + D3DXVECTOR3(radius * (float)cosf(a),		   0.0f, radius * (float)sinf(a)));
		verticesCircleBase.push_back(center1 + D3DXVECTOR3(radius * (float)cosf(a + step), 0.0f, radius * (float)sinf(a + step)));
		verticesCircleBase.push_back(center2 + D3DXVECTOR3(radius * (float)cosf(a),		   0.0f, radius * (float)sinf(a)));
		verticesCircleBase.push_back(center2 + D3DXVECTOR3(radius * (float)cosf(a + step), 0.0f, radius * (float)sinf(a + step)));
	}
	
	//round-round line
	size_t size = verticesCircleBase.size();
	for (size_t i = 0; i < size; i += 4)
	{
		verticesCircleBase.push_back(verticesCircleBase[i]);
		verticesCircleBase.push_back(verticesCircleBase[i + 2]);
	}
	//for (size_t i = 0; i < 4; i++)
	//{
	//	verticesCircleBase.push_back(center1 + D3DXVECTOR3(radius * (float)cosf(i * Math::PiOver4), 0.0f, radius * (float)sinf(i * Math::PiOver4)));
	//	verticesCircleBase.push_back(center2 + D3DXVECTOR3(radius * (float)cosf(i * Math::PiOver4), 0.0f, radius * (float)sinf(i * Math::PiOver4)));
	//}

	//sphere
	for (float a = 0.0f; a < Math::PI; a += step)
	{
		verticesCircleBase.push_back(center1 + D3DXVECTOR3(radius * (float)cosf(a),		   radius * (float)sinf(a),			0.0f));
		verticesCircleBase.push_back(center1 + D3DXVECTOR3(radius * (float)cosf(a + step), radius * (float)sinf(a + step),	0.0f));
		verticesCircleBase.push_back(center1 + D3DXVECTOR3(0.0f, radius * (float)sinf(a),		 radius * (float)cosf(a)));
		verticesCircleBase.push_back(center1 + D3DXVECTOR3(0.0f, radius * (float)sinf(a + step), radius * (float)cosf(a + step)));
	}
	for (float a = Math::PI; a < Math::TwoPI; a += step)
	{
		verticesCircleBase.push_back(center2 + D3DXVECTOR3(radius * (float)cosf(a),			radius * (float)sinf(a), 0.0f));
		verticesCircleBase.push_back(center2 + D3DXVECTOR3(radius * (float)cosf(a + step),	radius * (float)sinf(a + step), 0.0f));
		verticesCircleBase.push_back(center2 + D3DXVECTOR3(0.0f, radius * (float)sinf(a),		 radius * (float)cosf(a)));
		verticesCircleBase.push_back(center2 + D3DXVECTOR3(0.0f, radius * (float)sinf(a + step), radius * (float)cosf(a + step)));
	}
}

void BoundingCapsule::CalcVertices(void)
{
	verticesCircleBase.clear();
	InitVertices();
}

D3DXVECTOR3 BoundingCapsule::GetCenter(void)
{
	D3DXVECTOR3 res;
	D3DXVec3TransformCoord(&res, &center, &myWorld);
	return res;
}

void BoundingCapsule::GetCenters(D3DXVECTOR3 & c1, D3DXVECTOR3 & c2)
{
	D3DXVECTOR3 center1 = center + D3DXVECTOR3(0, 0.5f, 0) * height;
	D3DXVECTOR3 center2 = center + D3DXVECTOR3(0, -0.5f, 0) * height;

	D3DXVec3TransformCoord(&c1, &center1, &myWorld);
	D3DXVec3TransformCoord(&c2, &center2, &myWorld);
}

float BoundingCapsule::GetRadius(void)
{
	D3DXMATRIX mat;
	mymodel->GetWorld(mat);
	
	return radius * mat._11;// * mymodel->GetTransformRef().Scale.x;
}

float BoundingCapsule::GetHeight(void)
{
	D3DXMATRIX mat;
	mymodel->GetWorld(mat);

	return height * mat._22;// * mymodel->GetTransformRef().Scale.y;
}

bool BoundingCapsule::isCollision(BoundingCapsule * capsule1, BoundingCapsule * capsule2)
{
	float r1 = capsule1->GetRadius();
	float r2 = capsule2->GetRadius();
	float h1 = capsule1->GetHeight();
	float h2 = capsule2->GetHeight();

	//계산 전 예외처리
	{
		float maxDist = (h1 + h2) * 0.5f + r1 + r2;

		D3DXVECTOR3 center1, center2;
		center1 = capsule1->GetCenter();
		center2 = capsule2->GetCenter();

		float cenDist = D3DXVec3Length(&(center1 - center2));

		if (cenDist > maxDist)
			return false;
	}



	float r = r1 + r2;
	float dist = dist3D_Segment_to_Segment(capsule1, capsule2);

	if (dist <= r)
		return true;

	return false;
}

// dist3D_Segment_to_Segment(): get the 3D minimum distance between 2 segments
//    Input:  two 3D line segments S1 and S2
//    Return: the shortest distance between S1 and S2
float BoundingCapsule::dist3D_Segment_to_Segment(BoundingCapsule* S1, BoundingCapsule* S2)
{
	D3DXVECTOR3 S1P0, S1P1, S2P0, S2P1;
	S1->GetCenters(S1P0, S1P1);
	S2->GetCenters(S2P0, S2P1);
	
	D3DXVECTOR3  u = S1P1 - S1P0;
	D3DXVECTOR3  v = S2P1 - S2P0;
	D3DXVECTOR3  w = S1P0 - S2P0;
	D3DXVECTOR3	 z = S1P0 - S2P1;
	D3DXVECTOR3	 g = S1P1 - S2P0;
	D3DXVECTOR3	 h = S1P1 - S2P1;
	float		a = D3DXVec3Dot(&u, &u);         // always >= 0
	float		b = D3DXVec3Dot(&u, &v);
	float		c = D3DXVec3Dot(&v, &v);         // always >= 0

	float		d = D3DXVec3Dot(&u, &w);
	float		e = D3DXVec3Dot(&v, &w);
	float		D = a*c - b*b;        // always >= 0
	float		sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float		tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

								   // compute the line parameters of the two closest points
	if (D < SMALL_NUM) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (abs(sN) < SMALL_NUM ? 0.0f : sN / sD);
	tc = (abs(tN) < SMALL_NUM ? 0.0f : tN / tD);

	// get the difference of the two closest points
	D3DXVECTOR3 dP = w + (u * sc) - (v* tc);  // =  S1(sc) - S2(tc)

	float distance  = D3DXVec3Length(&dP);
	float wDistance = D3DXVec3Length(&w);
	float zDistance = D3DXVec3Length(&z);
	float gDistance = D3DXVec3Length(&g);
	float hDistance = D3DXVec3Length(&h);

	distance = Math::Min(distance, wDistance);
	distance = Math::Min(distance, zDistance);
	distance = Math::Min(distance, gDistance);
	distance = Math::Min(distance, hDistance);

	return distance;   // return the closest distance
}

