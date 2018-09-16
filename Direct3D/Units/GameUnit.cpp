#include "stdafx.h"
#include "GameUnit.h"
#include "GameData.h"

GameUnit::GameUnit(wstring matmeshFile, ANIMATION_TYPE animType)
	: GameModel(matmeshFile, animType)
{
	valid = true;
	isLive = true;

	unitInfo.Level = 0;
	unitInfo.HP = 0.0f;
	unitInfo.HPMax = 0.0f;
	unitInfo.MP = 0.0f;
	unitInfo.MPMax = 0.0f;
	unitInfo.Exp = 0.0f;
	unitInfo.ExpMax = 0.0f;
	unitInfo.Atk = 0.0f;
	unitInfo.Dep = 0.0f;
}

GameUnit::~GameUnit()
{
}

void GameUnit::Update(void)
{
	GameModel::Update();

}

void GameUnit::Render(void)
{
	GameModel::Render();

}


void GameUnit::ActionHit(GameUnit * victim)
{
}

void GameUnit::ActionDamage(GameUnit * attacker)
{
	float atk = attacker->unitInfo.Atk;
	float dep = this->unitInfo.Dep;
	float damage = Math::Max(0.0f, atk - dep);

	unitInfo.HP -= damage;

	//debug
	//unitInfo.HP = 0.0f;

	if (unitInfo.HP < 1.0f)
	{
		unitInfo.HP = 0.0f;
		isLive = false;
	}
}
