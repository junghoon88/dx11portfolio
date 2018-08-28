#pragma once

#include "../../Objects/GameModel.h"

enum class BoundingType
{
	Box, Sphere, Capsule, 
	Count,
};

enum class BoundingProp
{
	Unknown = 0, Weapon, Armor, FrustumCulling,
	Count,
};

class Bounding
{
protected:
	Bounding(GameModel* mymodel);
	virtual ~Bounding() {}

public:
	virtual void Update(void);
	virtual void PostRender(void);
	virtual void GetCorners(vector<D3DXVECTOR3>& vec) = 0;

public:
	inline GameModel* GetMyModel(void) { return mymodel; }
	inline int GetSocketnum(void) { return socketnum; }
	inline void SetSocketnum(int value) { socketnum = value; }
	inline string GetName(void) { return name; }
	inline void SetName(string value) { name = value; }

	inline D3DXCOLOR& GetColorRef(void) { return color; }
	inline void SetColor(D3DXCOLOR value) { color = value; }
	inline BoundingType GetBoundingType(void) { return type; }
	inline BoundingProp GetBoundingProp(void) { return prop; }
	inline void SetBoundingProp(BoundingProp prop) { this->prop = prop; }


	inline TRANSFORM& GetTransformRef(void) { return transform; }
	inline void SetTransform(TRANSFORM& value) { transform = value; }
	inline void SetSocketTransform(D3DXMATRIX& mat) { socketTransform = mat; }
	inline D3DXMATRIX GetMyWorld(void) { return myWorld; }

protected:
	GameModel*	mymodel;
	int			socketnum;

	string		name;
	D3DXCOLOR	color;
	bool		bShow;

	BoundingType type;
	BoundingProp prop;

	TRANSFORM	transform;			// 자기자신의 월드
	D3DXMATRIX	socketTransform;	// 모델이 있을 때 모델의 소캣 월드
	D3DXMATRIX	myWorld;			// 모델의 월드까지 감안한 최종 월드
};
