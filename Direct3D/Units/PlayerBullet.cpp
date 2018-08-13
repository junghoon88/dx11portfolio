#include "stdafx.h"
#include "PlayerBullet.h"
#include "../Model/Bounding/BoundingSphere.h"
#include "../Model/Bounding/ModelShape.h"

PlayerBullet::PlayerBullet(wstring modelPath, BulletType type, D3DXVECTOR3 pos, D3DXVECTOR3 dir)
	: GameModel(modelPath)
	, isValid(true)
	, type(type)
	, speed(30.0f)
	, range(20.0f)
	, position(pos)
	, positionFired(pos)
	, direction(dir)
{
	sphere = new BoundingSphere();
	//sphere->Radius = ;
}

PlayerBullet::~PlayerBullet()
{
	SAFE_DELETE(sphere);
}

void PlayerBullet::Update(ModelShape* modelShape)
{
	switch (type)
	{
		case PlayerBullet::BulletType::Straight:
		{
			position += direction * speed * Time::Delta();
		}
		break;
		case PlayerBullet::BulletType::Parabola:
		{
			direction.y += -0.07f;
			position += direction * speed * Time::Delta();

			if (position.y < 0.0f)
			{
				isValid = false;
				return;
			}
		}
		break;
	}

	//거리체크
	{
		D3DXVECTOR2 pos1, pos2;
		pos1 = D3DXVECTOR2(positionFired.x, positionFired.z);
		pos2 = D3DXVECTOR2(position.x, position.z);

		float length = D3DXVec2Length(&(pos2 - pos1));
		if (length > range)
		{
			isValid = false;
			return;
		}
	}
	D3DXMATRIX P;
	D3DXMatrixTranslation(&P, position.x, position.y, position.z);
	sphere->SetMyWorld(P);

	//modelShape->AddBoundingSphere(sphere, MathDX::Red);
}

void PlayerBullet::Render(void)
{
}

