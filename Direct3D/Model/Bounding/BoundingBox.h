#pragma once
#include "Bounding.h"

class BoundingBox : public Bounding
{
public:

	//
	BoundingBox(GameModel* mymodel, D3DXVECTOR3 min, D3DXVECTOR3 max);
	~BoundingBox();

	virtual void Update(void) override;
	virtual void PostRender(void) override;
	virtual void GetCorners(vector<D3DXVECTOR3>& vec) override;

	D3DXVECTOR3 GetMin(void) const;
	D3DXVECTOR3 GetMax(void) const;

	inline D3DXVECTOR3 GetOrgMin(void) const { return Min; }
	inline D3DXVECTOR3 GetOrgMax(void) const { return Max; }

	inline void SetMin(D3DXVECTOR3 vec) { Min = vec; }
	inline void SetMax(D3DXVECTOR3 vec) { Max = vec; }

public:

private:
	const int	CornerCount;
	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;
};

