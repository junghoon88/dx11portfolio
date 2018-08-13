#pragma once

class GameUnit;

class BoundingSphere
{
public:
	friend class BoundingFrustum;

	D3DXVECTOR3			Center;
	float				Radius;

	//
	BoundingSphere();
	~BoundingSphere();

	void Update(void);
	void PostRender(void);

	void GetCorners(vector<D3DXVECTOR3>& vec);

private:
	void InitVertices(void);

public:
	inline void SetMyUnit(GameUnit* unit) { myUnit = unit; }
	inline void SetSocketNum(UINT val) { socketNum = val; }
	inline int GetSocketNum(void) { return socketNum; }
	inline void SetMyWorld(D3DXMATRIX& mat) { myWorld = mat; }

private:
	//sphere
	int					sphereResolution;
	int					sphereLineCount;
	vector<D3DXVECTOR3> unitSphere;


	GameUnit*			myUnit;
	int					socketNum;
	TRANSFORM			transform;
	D3DXMATRIX			socketTransform;
	D3DXMATRIX			myWorld;


};
