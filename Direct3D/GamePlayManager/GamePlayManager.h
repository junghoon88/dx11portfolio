#pragma once

//���� ����ð�, ���� �ɼ�, ���� ������ ���� 
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

