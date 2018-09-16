#pragma once

//게임 진행시간, 게임 옵션, 몬스터 리스폰 관리 
#include "EnemyRespawnManager.h"

class DrawStage;

class GamePlayManager
{
public:
	GamePlayManager(DrawStage* stage);
	~GamePlayManager();

	void Update(void);

public:
	inline EnemyRespawnManager*	GetEnemyRespawnManager(void) { return enemyRespawnManager; }

private:
	DrawStage*				stage;
	float					playTime;
	EnemyRespawnManager*	enemyRespawnManager;
};

