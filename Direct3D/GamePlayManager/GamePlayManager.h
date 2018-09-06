#pragma once

//게임 진행시간, 게임 옵션, 몬스터 리스폰 관리 

class DrawStage;
class EnemyRespawnManager;

class GamePlayManager
{
public:
	GamePlayManager(DrawStage* stage);
	~GamePlayManager();

private:
	DrawStage*				stage;
	float					playTime;
	EnemyRespawnManager*	enemyRespawnManager;
};

