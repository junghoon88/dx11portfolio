#pragma once

class BoundingBox
{
public:

	//
	BoundingBox(D3DXVECTOR3 min, D3DXVECTOR3 max);
	~BoundingBox();

	vector<D3DXVECTOR3> GetCorners(void);
	D3DXVECTOR3 GetMin(void) const;
	D3DXVECTOR3 GetMax(void) const;
	D3DXVECTOR3 GetOrgMin(void) const { return Min; }
	D3DXVECTOR3 GetOrgMax(void) const { return Max; }

public:
	inline TRANSFORM& GetTransformRef(void) { return transform; }
	inline void SetTransform(TRANSFORM& value) { transform = value; }
	inline void SetSocketTransform(D3DXMATRIX& mat) { socketTransform = mat; }
	inline D3DXMATRIX& GetSocketTransform(void) { return socketTransform; }

private:
	const int	CornerCount;
	D3DXVECTOR3 Min;
	D3DXVECTOR3 Max;

	TRANSFORM			transform;
	D3DXMATRIX			socketTransform;
	D3DXMATRIX			myWorld;

};

