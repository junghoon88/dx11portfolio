#include "stdafx.h"
#include "GameSettings.h"
#include "GamePlayer.h"

#include "../Landscape/Terrain.h"

GamePlayer* GameSettings::player = NULL;
Landscape::Terrain* GameSettings::terrain = NULL;


GameSettings::GameSettings(ExecuteValues* values)
	: values(values)
{
	CreateEnvironment();
	CreatePlayer();
	CreateEnemy();

}

GameSettings::~GameSettings()
{
	SAFE_DELETE(terrain);
	SAFE_DELETE(player);
}

void GameSettings::Update(void)
{
	if (terrain) terrain->Update();

	if (player) player->Update();
}

void GameSettings::Render()
{
	if (terrain) terrain->Render();

	if (player) player->Render();
}

void GameSettings::PostRender(void)
{
	if (terrain) terrain->PostRender();

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
}

