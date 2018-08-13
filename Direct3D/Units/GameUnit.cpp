#include "stdafx.h"
#include "GameUnit.h"
#include "GameWeapon.h"
#include "GameData.h"

GameUnit::GameUnit(wstring path)
	: GameAnimationModel(path)
	, curWeapon(NULL)
	, curWeaponSlot(-1)
{
	min = D3DXVECTOR3(Math::FloatMaxValue, Math::FloatMaxValue, Math::FloatMaxValue);
	max = D3DXVECTOR3(Math::FloatMinValue, Math::FloatMinValue, Math::FloatMinValue);
	model->ScanPointMinMax(&min, &max);

	AddClipAll(path);

	life = maxLife = 0;

	D3DXMatrixIdentity(&spawnPoint);
}

GameUnit::~GameUnit()
{
}

void GameUnit::Update(ModelShape* modelShape)
{
	GameAnimationModel::Update(modelShape);
}

void GameUnit::Render(void)
{
	GameAnimationModel::Render();
}

void GameUnit::Attack(GameUnit * target)
{
}

void GameUnit::Hit(GameUnit * target)
{
}

bool GameUnit::MousePickked(BoundingFrustum * cameraFrustum)
{
	return false;
}

bool GameUnit::MousePickked(D3DXVECTOR3 start, D3DXVECTOR3 dir, OUT map<float, ModelMeshPart*>& pickkedParts)
{
	float dist = 0.0f;

	for (UINT i = 0; i < model->GetMeshCount(); i++)
	{
		//for (ModelMeshPart* part : model->GetMeshesRef()[i]-> meshParts)
		//{
		//	if (part->MousePickked(start, direction, dist))
		//	{
		//		pickkedParts[dist] = part;
		//	}
		//}
	}

	if (pickkedParts.size() > 0)
		return true;

	return false;
}

void GameUnit::CreateWeapon(wstring matFile, wstring meshFile)
{
	GameWeaponSpec* spec = new GameWeaponSpec();
	//TODO : 스펙 데이터 로딩

	GameWeapon* weapon = new GameWeapon();
	weapon->SetSpecData(spec);
	weapons.push_back(weapon);
	//TODO : 무기 장착

	SelectWeapon(0);
}

void GameUnit::SelectWeapon(UINT slot)
{
	assert(slot < weapons.size());
	curWeapon = weapons[slot];
	curWeaponSlot = slot;

	for (size_t i = 0; i < weapons.size(); i++)
	{
		if (i == slot)
		{
			weapons[i]->SetVisible(true);
			weapons[i]->SetEnable(true);
		}
		else
		{
			weapons[i]->SetVisible(false);
			weapons[i]->SetEnable(false);
		}
	}
}

