#pragma once

//���� ����ð�, ���� �ɼ�, ���� ������ ���� 

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

