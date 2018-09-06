#include "stdafx.h"
#include "GamePlayManager.h"
#include "EnemyRespawnManager.h"
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
