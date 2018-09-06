#pragma once
#include "Bounding.h"

class BoundingCapsule : public Bounding
{
public:
	BoundingCapsule(GameModel* mymodel, D3DXVECTOR3 center, float radius, float height);
	~BoundingCapsule();

	virtual void Update(void) override;
	virtual void PostRender(void) override;
	virtual void GetCorners(vector<D3DXVECTOR3>& vec) override;

	void SetCenter(D3DXVECTOR3 vec);
	void SetRadius(float r);
	void SetHeight(float h);

	void CalcVertices(void);

private:
	void InitVertices(void);
	D3DXVECTOR3 GetCenter(void);
	void GetCenters(D3DXVECTOR3& c1, D3DXVECTOR3& c2);
	float GetRadius(void);
	float GetHeight(void);

public:
	static bool isCollision(BoundingCapsule* c1, BoundingCapsule* c2);

private:
	static float dist3D_Segment_to_Segment(BoundingCapsule* S1, BoundingCapsule* S2);


public:
	inline D3DXVECTOR3& GetCenterRef(void) { return center; }
	inline float& GetRadiusRef(void) { return radius; }
	inline float& GetHeightRef(void) { return height; }

private:
	D3DXVECTOR3 center;
	float radius;
	float height;

	//vertex
	const float			sphereResolution;
	vector<D3DXVECTOR3> verticesCircleBase;
};
