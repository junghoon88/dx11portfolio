#pragma once

class BoundingSphere;
class BoundingCylinder;
class GameUnit;

class BoundingCapsule
{
public:
	BoundingCapsule(GameUnit* mymodel, D3DXVECTOR3 center, float radius, float height);
	~BoundingCapsule();

	void Update(void);
	void PostRender(void);

	void SetCenter(D3DXVECTOR3 vec);
	void SetRadius(float r);
	void SetHeight(float h);

	void GetCorners(vector<D3DXVECTOR3>& vec);
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
	inline GameUnit* GetMyModel(void) { return mymodel; }
	inline int GetSocketnum(void) { return socketnum; }
	inline void SetSocketnum(int value) { socketnum = value; }
	inline string GetName(void) { return string(name); }
	inline void SetName(string value) { strcpy_s(name, 128, value.c_str()); }
	inline D3DXVECTOR3& GetCenterRef(void) { return center; }
	inline float& GetRadiusRef(void) { return radius; }
	inline float& GetHeightRef(void) { return height; }

	inline D3DXCOLOR& GetColorRef(void) { return color; }
	inline void SetColor(D3DXCOLOR value) { color = value; }
	inline bool GetIsWeapon(void) { return isWeapon; }
	inline void SetIsWeapon(bool value) { isWeapon = value; }
	inline bool GetIsArmor(void) { return isArmor; }
	inline void SetIsArmor(bool value) { isArmor = value; }

	inline TRANSFORM& GetTransformRef(void) { return transform; }
	inline void SetTransform(TRANSFORM& value) { transform = value; }
	inline void SetSocketTransform(D3DXMATRIX& mat) { socketTransform = mat; }


private:
	GameUnit* mymodel;
	int		 socketnum;

	D3DXVECTOR3 center;
	float radius;
	float height;

	char name[128];
	D3DXCOLOR color;
	bool isWeapon;
	bool isArmor;

	//vertex
	const float			sphereResolution;
	vector<D3DXVECTOR3> verticesCircleBase;

	TRANSFORM			transform;
	D3DXMATRIX			socketTransform;
	D3DXMATRIX			myWorld;
};
