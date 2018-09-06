#pragma once

#include "Bounding.h"

class BoundingSphere : public Bounding
{
public:
	friend class BoundingFrustum;

	D3DXVECTOR3			Center;
	float				Radius;

	//
	BoundingSphere(GameModel* mymodel);
	~BoundingSphere();

	virtual void Update(void) override;
	virtual void PostRender(void) override;
	virtual void GetCorners(vector<D3DXVECTOR3>& vec) override;

private:
	void InitVertices(void);

private:
	//sphere
	int					sphereResolution;
	int					sphereLineCount;
	vector<D3DXVECTOR3> unitSphere;
};
