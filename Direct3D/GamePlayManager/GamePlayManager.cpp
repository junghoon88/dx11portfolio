#include "stdafx.h"
#include "GamePlayManager.h"
#include "../Executes/DrawStage.h"

GamePlayManager::GamePlayManager(DrawStage * stage)
	: stage(stage)
	, playTime(0.0f)
{
	enemyRespawnManager = new EnemyRespawnManager(stage);
}

GamePlayManager::~GamePlayManager()
{
	SAFE_DELETE(enemyRespawnManager);
}

void GamePlayManager::Update(void)
{
	if (enemyRespawnManager->GetBattleStart() == false)
	{
		enemyRespawnManager->SetBattleStart(true);
		return;
	}
	else if (enemyRespawnManager->GetBattleEnd() == true)
	{
		//Battle Á¾·á
	}
	else
	{
		playTime += gTime->Delta();
		enemyRespawnManager->Update();
	}

}
