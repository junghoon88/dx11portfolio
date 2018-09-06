#include "stdafx.h"
#include "GameUnit.h"
#include "GameData.h"

GameUnit::GameUnit(wstring matmeshFile, ANIMATION_TYPE animType)
	: GameModel(matmeshFile, animType)
{
	Level = 0;
	HP = 0.0f;
	HPMax = 0.0f;
	MP = 0.0f;
	MPMax = 0.0f;
	Exp = 0.0f;
	ExpMax = 0.0f;
	Atk = 0.0f;
	Dep = 0.0f;
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

void GameUnit::Attack(GameUnit* target)
{
}

void GameUnit::Hit(GameUnit* target)
{
}
