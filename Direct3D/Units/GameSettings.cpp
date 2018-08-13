#include "stdafx.h"
#include "GameSettings.h"
#include "GamePlayer.h"
#include "EnemyTank.h"

#include "../Landscape/Terrain.h"

GamePlayer* GameSettings::player = NULL;
Landscape::Terrain* GameSettings::terrain = NULL;


GameSettings::GameSettings(ExecuteValues* values)
	: values(values)
	, tank(NULL)
{
	CreateEnvironment();
	CreatePlayer();
	CreateEnemy();

}

GameSettings::~GameSettings()
{
	SAFE_DELETE(terrain);
	SAFE_DELETE(tank);
	SAFE_DELETE(player);
}

void GameSettings::Update(ModelShape* modelShape)
{
	if (terrain) terrain->Update();

	if (tank)	tank->Update(modelShape);
	if (player) player->Update(modelShape);
}

void GameSettings::Render()
{
	if (terrain) terrain->Render();

	if (tank)	tank->Render();
	if (player) player->Render();
}

void GameSettings::PostRender(void)
{
	if (terrain) terrain->PostRender();

	if (tank)	 tank->PostRender();
}

void GameSettings::CreateEnvironment(void)
{
	terrain = new Landscape::Terrain(values);
}

void GameSettings::CreatePlayer(void)
{
	//grund
	player = new GamePlayer(Models + L"RobotGame/Grund/");
}

void GameSettings::CreateEnemy(void)
{
	tank = new EnemyTank(Models + L"RobotGame/Tiger/");
	tank->SetStartAi(GameEnemy::AiType::Move, 1);

	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, Math::ToRadian(180));
	tank->SetRootAxis(R);
}

