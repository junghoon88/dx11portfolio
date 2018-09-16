#include "stdafx.h"
#include "GameEnemyRobot.h"
#include "GamePlayer.h"
#include "GameData.h"
#include "AiContext.h"
#include "AiState.h"

#include "../Model/Bounding/BoundingBox.h"
#include "../Model/Bounding/Ray.h"

GameEnemyRobot::GameEnemyRobot(wstring matmeshFile)
	: GameEnemy(matmeshFile)
	, attackDelay(0.0f)
{
	specData = new GameEnemyRobotSpec();

	unitInfo.Level	= 1;
	unitInfo.HP		= specData->HP;
	unitInfo.HPMax	= specData->HP;
	unitInfo.MP		= 0.0f;
	unitInfo.MPMax	= 0.0f;
	unitInfo.ExpMax = 0.0f;
	unitInfo.Atk	= specData->Atk;
	unitInfo.Dep	= specData->Dep;

}

GameEnemyRobot::~GameEnemyRobot()
{
	SAFE_DELETE(specData);
}

void GameEnemyRobot::Init(void)
{
	LinkAnimation();

	PlayAction(EnemyRobotAction::Idle);
}

void GameEnemyRobot::ActionHit(GameUnit * victim)
{
}

void GameEnemyRobot::ActionDamage(GameUnit * attacker)
{
	GameUnit::ActionDamage(attacker);

	if (isLive == false)
	{
		SetStartAi(AiType::Death);
		PlayAction(EnemyRobotAction::Death);
	}
	else
	{
		SetStartAi(AiType::Damage);
		PlayAction(EnemyRobotAction::Damage);
	}
}


void GameEnemyRobot::LinkAnimation(void)
{
	UINT animCount = (UINT)EnemyRobotAction::Count;

	indexAnimations.assign(animCount, UINT(-1));

	// Animation
	{
		indexAnimations[(UINT)EnemyRobotAction::Idle]   = FindClip(L"Idle");
		indexAnimations[(UINT)EnemyRobotAction::Run]    = FindClip(L"Running");
		indexAnimations[(UINT)EnemyRobotAction::Paunch] = FindClip(L"Punching");
		indexAnimations[(UINT)EnemyRobotAction::Kick]   = FindClip(L"Mma Kick");
		indexAnimations[(UINT)EnemyRobotAction::Damage] = FindClip(L"Reaction");
		indexAnimations[(UINT)EnemyRobotAction::Death]  = FindClip(L"Death");
	}

	for (UINT i = 0; i < animCount; i++)
	{
		assert(indexAnimations[i] != UINT(-1));
	}

	//runDurationTime			 = GetClip(indexAnimations[(UINT)EnemyRobotAction::Run])->Duration;
	//walkDurationTime		 = GetClip(indexAnimations[(UINT)EnemyRobotAction::Walk])->Duration;
	//weaponChangeDurationTime = GetClip(indexAnimations[(UINT)EnemyRobotAction::WeaponChange])->Duration;
}

void GameEnemyRobot::ActionIdle(void)
{
	GameModel::velocity = D3DXVECTOR3(0, 0, 0);
}

void GameEnemyRobot::ActionFollow(D3DXVECTOR3 dir, float dist)
{
	if (curAction != EnemyRobotAction::Run)
		PlayAction(EnemyRobotAction::Run);

	LookatPlayer(dir);
	GameModel::velocity = D3DXVECTOR3(0, 0, 1) * specData->MoveSpeed;
}

void GameEnemyRobot::ActionAttack(D3DXVECTOR3 dir, float dist)
{
	if (attackDelay <= 0.0f)
	{
		PlayAction(EnemyRobotAction::Paunch);
		attackDelay = specData->AttackDelay;
	}
	else
	{
		LookatPlayer(dir);
	}
	GameModel::velocity = D3DXVECTOR3(0, 0, 0);
}

void GameEnemyRobot::ActionDamage(void)
{
	GameModel::velocity = D3DXVECTOR3(0, 0, 0);
}

void GameEnemyRobot::ActionDeath(void)
{
	GameModel::velocity = D3DXVECTOR3(0, 0, 0);
}

void GameEnemyRobot::Update(void)
{
	if (valid == false) return;

	if (attackDelay > 0.0f)
		attackDelay -= gTime->Delta();

	GameEnemy::Update();

	if (GameModel::isPlayEnd)
	{
		if (isLive)
		{
			SetStartAi(AiType::Idle);
			PlayAction(EnemyRobotAction::Idle);
		}
		else
		{
			valid = false;
		}
	}
}

void GameEnemyRobot::Render(void)
{
	if (valid == false) return;

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
		SetStartAi(AiType::Attack);
	}
	else if (dist <= specData->FollowRange)
	{
		//Follow
		SetStartAi(AiType::Follow);
	}
	else
	{
		//Idle
		ActionIdle();
	}
}

void GameEnemyRobot::OnAiFollow(AiState * state)
{
	D3DXVECTOR3 dir;
	float dist;
	GameEnemy::CalcDistancePlayer(dir, dist);

	if (dist <= specData->AttackRange)
	{
		//Attack
		SetStartAi(AiType::Attack);
	}
	else if (dist <= specData->FollowRange)
	{
		//Follow
		ActionFollow(dir, dist);
	}
	else
	{
		//Idle
		SetStartAi(AiType::Idle);
	}
}

void GameEnemyRobot::OnAiAttack(AiState * state)
{
	D3DXVECTOR3 dir;
	float dist;
	GameEnemy::CalcDistancePlayer(dir, dist);

	if (dist <= specData->AttackRange)
	{
		//Attack
		ActionAttack(dir, dist);
	}
	else if (dist <= specData->FollowRange)
	{
		//Follow
		SetStartAi(AiType::Follow);
	}
	else
	{
		//Idle
		SetStartAi(AiType::Idle);
	}
}

void GameEnemyRobot::OnAiDamage(AiState * state)
{
	ActionDamage();
}

void GameEnemyRobot::OnAiDeath(AiState * state)
{
	ActionDeath();
}

void GameEnemyRobot::PlayAction(EnemyRobotAction action, float startTime)
{
	float blendTime = 0.0f;

	switch (action)
	{
		case EnemyRobotAction::Idle:
		{
			for (Bounding* bounding : GameModel::GetBoundingsRef())
			{
				if (bounding->GetBoundingProp() == BoundingProp::Weapon)
				{
					bounding->SetShow(false);
					bounding->SetEnable(false);
				}
				else if (bounding->GetBoundingProp() == BoundingProp::Armor)
				{
					bounding->SetShow(true);
					bounding->SetEnable(true);
				}
			}
			blendTime = 0.5f;
		}
		break;

		case EnemyRobotAction::Run:
			blendTime = 0.3f;
			break;

		case EnemyRobotAction::Paunch:
		{
			Bounding* paunch = GameModel::GetBounding("Paunch");
			if (paunch != NULL)
			{
				paunch->SetShow(true);
				paunch->SetEnable(true);
			}
			blendTime = 0.2f;
		}
		break;
		case EnemyRobotAction::Kick:
		{
			Bounding* kick = GameModel::GetBounding("Kick");
			if (kick != NULL)
			{
				kick->SetShow(true);
				kick->SetEnable(true);
			}
			blendTime = 0.2f;
		}
		break;

		case EnemyRobotAction::Damage:
		{

		}
		break;

		case EnemyRobotAction::Death:
		{
			for (Bounding* bounding : GameModel::GetBoundingsRef())
			{
				if (bounding->GetBoundingProp() == BoundingProp::Weapon)
				{
					bounding->SetShow(false);
					bounding->SetEnable(false);
				}
				else if (bounding->GetBoundingProp() == BoundingProp::Armor)
				{
					bounding->SetShow(false);
					bounding->SetEnable(false);
				}
			}

			blendTime = 0.0f;
		}
		break;
	}

	PlayAnim(indexAnimations[(UINT)action], startTime, blendTime, 1.0f);
	curAction = action;
	//nextAction = action;
}

void GameEnemyRobot::LookatPlayer(D3DXVECTOR3 dir)
{
	D3DXVECTOR3 vec, myDir;
	GameRender::GetDirection(myDir);

	D3DXVec3Cross(&vec, &dir, &myDir);

	if (vec.y > 0.0f)
	{
		GameModel::Rotate(D3DXVECTOR2(-180.0f, 0));
	}
	else
	{
		GameModel::Rotate(D3DXVECTOR2(180.0f, 0));
	}
}
