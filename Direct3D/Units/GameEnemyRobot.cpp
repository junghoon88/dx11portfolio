#include "stdafx.h"
#include "GameEnemyRobot.h"
#include "GamePlayer.h"
#include "GameData.h"
#include "AiContext.h"
#include "AiState.h"

GameEnemyRobot::GameEnemyRobot(wstring matmeshFile)
	: GameEnemy(matmeshFile)
{
	specData = new GameEnemyRobotSpec();
}

GameEnemyRobot::~GameEnemyRobot()
{
	SAFE_DELETE(specData);
}

void GameEnemyRobot::Update(void)
{
	GameEnemy::Update();
}

void GameEnemyRobot::Render(void)
{
	GameEnemy::Render();
}

void GameEnemyRobot::OnAiIdle(AiState * state)
{
	D3DXVECTOR3 dir;
	float dist;
	GameEnemy::CalcDistancePlayer(dir, dist);
	if (dist <= specData->AttackRange)
	{
		//Attack
		SetStartAi(AiType::Attack, Math::Random(2.0f, 3.0f));
	}
	else if (dist <= specData->FollowRange)
	{
		//Follow
		SetStartAi(AiType::Follow, Math::Random(3.0f, 4.0f));
	}
	else
	{
		//Idle
		float angle = atan2(dir.z, dir.x);
		GameModel::Rotate(D3DXVECTOR2(angle, 0));
	}
}

void GameEnemyRobot::OnAiFollow(AiState * state)
{

}

void GameEnemyRobot::OnAiAttack(AiState * state)
{
}
