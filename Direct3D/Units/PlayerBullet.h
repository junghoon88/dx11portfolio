#pragma once

class BoundingSphere;
class ModelShape;

#include "../Objects/GameModel.h"

class PlayerBullet : public GameModel
{
public:
	enum class BulletType
	{
		Straight = 0, Parabola
	};
	struct BulletInfo
	{
		BulletType Type;
		D3DXVECTOR3 PosFired;
		D3DXVECTOR3 Dir;
		float Speed;
		float Range;
	};
public:
	PlayerBullet(wstring modelPath, BulletType type, D3DXVECTOR3 pos, D3DXVECTOR3 dir);
	~PlayerBullet();

	virtual void Update(ModelShape* modelShape) override;
	virtual void Render(void) override;
	virtual void PostRender(void) override {}


public:
	inline bool GetIsValid(void) { return isValid; }

private:
	bool isValid;

	BulletType type;

	float speed;
	float range;
	D3DXVECTOR3 position;
	D3DXVECTOR3 positionFired;
	D3DXVECTOR3 direction;

	BoundingSphere* sphere;
};
