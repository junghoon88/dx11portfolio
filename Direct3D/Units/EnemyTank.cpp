#include "stdafx.h"
#include "EnemyTank.h"
#include "AiContext.h"
#include "GameSettings.h"
#include "GameData.h"
#include "GameWeapon.h"

EnemyTank::EnemyTank(wstring path)
	: GameEnemy(path)
	, turretAngleSpeed(0.0f)
{
	curAction = Action::Unknown;
	engageAction = Action::Unknown;

	//json으로
	specData = new GameEnemySpec();
	specData->Type = UnitTypeId::Tiger;
	specData->Class = UnitClassId::Tank;
	specData->Life = 1300;
	specData->MechRadius = 2.8f;
	specData->MoveSpeed = 4.0f;
	specData->TurnAngle = 20.0f;

	curWeapon->GetSpecData()->Type = WeaponType::TigerCannon;
	curWeapon->GetSpecData()->Damage = 300;
	curWeapon->GetSpecData()->FireRange = 70.0f;
	curWeapon->GetSpecData()->TotalBulletCount = -1;
	curWeapon->GetSpecData()->ReloadBulletCount = 5;
	curWeapon->GetSpecData()->FireIntervalTime = 3.0f;
	curWeapon->GetSpecData()->ReloadIntervalTime = 4.0f;
	curWeapon->GetSpecData()->FireHorizontalTilltAngle = 2;
	curWeapon->GetSpecData()->FireVerticalTilltAngle = 2;
	curWeapon->GetSpecData()->FireDelayTimeTillFirst = 0.0f;
	curWeapon->GetSpecData()->CriticalDamagedFire = true;
	curWeapon->GetSpecData()->ModelAlone = true;
	curWeapon->GetSpecData()->ModelCount = 0;
	curWeapon->GetSpecData()->TracerBulletFire = true;
	curWeapon->GetSpecData()->TracerBulletSpeed = 200;
	curWeapon->GetSpecData()->TracerBulletLength = 3.0f;
	curWeapon->GetSpecData()->TracerBulletThickness = 0.7f;

	UINT boneCount = model->GetBoneCount();
	for (UINT i = 0; i < boneCount; i++)
	{
		ModelBone* bone = model->GetBone(i);
		if (bone->GetName() == L"GunDummy")
			indexFireWeaponBone = bone->GetIndex();
	}

	indexTurretBone = model->GetBone(L"Top")->GetIndex();
	transformTurret = model->GetBone(indexTurretBone)->GetTransform();

	PlayAction(Action::Idle);

}

EnemyTank::~EnemyTank()
{
	SAFE_DELETE(specData);
}

void EnemyTank::Update(ModelShape* modelShape)
{
	ProcessAction();

	bool bCheck = true;
	bCheck &= engageAction != Action::Unknown;
	bCheck &= engageAction != curAction;

	if (bCheck)
		PlayAction(engageAction);

	if (turretAngleSpeed != 0.0f)
	{
		float rotate = turretAngleSpeed * Time::Delta();

		D3DXVECTOR3 right = MathDX::GetRight(transformTurret);
		
		D3DXMATRIX RA;
		D3DXMatrixRotationAxis(&RA, &right, Math::ToRadian(rotate));

		transformTurret *= RA;
		model->GetBone(indexTurretBone)->SetTransform(transformTurret * RA);
	}


	GameEnemy::Update(modelShape);
}

void EnemyTank::Render(void)
{
	GameEnemy::Render();
}

void EnemyTank::PostRender(void)
{
	ImGui::Text("Tank State : %d, %d", (int)engageAction, (int)curAction);

	D3DXVECTOR3 pPos, myPos;
	GameSettings::GetPlayer()->GetPosition(pPos);
	this->GetPosition(myPos);
	float dist = MathDX::Distance(pPos, myPos);

	ImGui::InputFloat("dist", &dist);
}

void EnemyTank::ActionIdle(void)
{
	engageAction = Action::Idle;
}

bool EnemyTank::ActionFire(void)
{
	D3DXVECTOR3 fireStart;
	fireStart = MathDX::GetPosition(boneTransforms[indexTurretBone]);

	D3DXVECTOR3 fireDir;
	fireDir = MathDX::GetPosition(boneTransforms[indexTurretBone]);

	return false;
}

void EnemyTank::ActionHit(GameUnit* attacker)
{
	if (IsDead())
		return;

	GameWeaponSpec* specData = attacker->GetCurWeapon()->GetSpecData();
	GamePlayer::DebugMode mode = GameSettings::GetPlayer()->GetDebugMode();

	bool bSuper = false;
	bSuper |= (mode == GamePlayer::DebugMode::Superman);
	bSuper |= (mode == GamePlayer::DebugMode::God);
	if (bSuper)
	{
		life = 0.0f;
	}
	else
	{
		life -= specData->Damage;
		if (life < 1.0f)
			life = 0.0f;
	}

	if (life < 1.0f)
	{
		D3DXVECTOR3 pos;
		attacker->GetPosition(pos);
		ActionDead(pos);
	}
}

bool EnemyTank::ActionReload(GameWeapon* weapon)
{
	//TODO : 재장전 처리

	return false;
}

void EnemyTank::ActionDamage(GameUnit* attacker)
{
	engageAction = Action::Damage;
	actionElapsedTime = 0.0f;
}

void EnemyTank::ActionDead(D3DXVECTOR3 attackerPosition)
{
	engageAction = Action::Dead;
	velocity = D3DXVECTOR3(0, 0, 0);

	turretAngleSpeed = 0.0f;
	aiContext->Enable(false);
}


void EnemyTank::ProcessAction(void)
{
	switch (curAction)
	{
		case Action::Idle:
		break;
		case Action::Damage:
		{
			ActionIdle();
			actionElapsedTime = 0.0f;
		}
		break;
		case Action::Move:
		{

		}
		break;
		case Action::Fire:
		{
			if (actionElapsedTime >= curWeapon->GetSpecData()->FireIntervalTime)
			{
				ActionIdle();
				actionElapsedTime = 0.0f;
			}
			else
			{
				actionElapsedTime += Time::Delta();
			}
		}
		break;
		case Action::Reload:
		{
			if (actionElapsedTime >= curWeapon->GetSpecData()->ReloadIntervalTime)
			{
				ActionIdle();
				actionElapsedTime = 0.0f;
			}
			else
			{
				actionElapsedTime += Time::Delta();
			}
		}
		break;
		case Action::LeftTrun:
		break;
		case Action::RightTurn:
		break;
		case Action::Dead:
		{
			//TODO : 사망처리
			
		}
		break;
	}
}

void EnemyTank::OnAiSearch(AiState* state)
{
	if (state->IsActive())
	{
		D3DXVECTOR3 pPos, myPos;
		GameSettings::GetPlayer()->GetPosition(pPos);
		this->GetPosition(myPos);
		float dist = MathDX::Distance(pPos, myPos);

		D3DXVECTOR3 direction;
		direction = pPos - myPos;
		D3DXVec3Normalize(&direction, &direction);

		D3DXMATRIX turret = boneTransforms[indexTurretBone];// model->GetBone(indexTurretBone)->GetTransform();
		D3DXVECTOR3 forward = MathDX::GetForward(turret);
		//D3DXVec3Normalize(&forward, &forward);

		D3DXMATRIX RY;
		D3DXMatrixRotationY(&RY, Math::ToRadian(-90));
		D3DXVec3TransformNormal(&forward, &forward, &RY);


		float dot = D3DXVec3Dot(&forward, &direction);
		float angleDeg = Math::ToDegree(acosf(dot));

		D3DXVECTOR3 temp;
		D3DXVec3Cross(&temp, &forward, &direction);
		if (temp.y < 0) 
			angleDeg *= -1;


		if (fabsf(angleDeg) < 5.0f)
		{
			if (dist <= curWeapon->GetSpecData()->FireRange)
			{
				//TODO : 공격
				NextAi(AiType::Attack, 1.0f);
			}
			else
			{
				NextAi(AiType::Search, 1.0f);
			}
		}
		else
		{
			if (angleDeg > 0)
				NextAi(AiType::TurnRight, 0.01f);// fabsf(angleDeg) / specData->TurnAngle);
			else
				NextAi(AiType::TurnLeft, 0.01f);// fabsf(angleDeg) / specData->TurnAngle);
		}

		turretAngleSpeed = 0.0f;
	}
	else
	{
		SetStartAi(AiType::Search, 1.0f);
		printf("");
	}

	velocity = D3DXVECTOR3(0, 0, 0);
}

void EnemyTank::OnAiMove(AiState* state)
{
	if (state->IsActive())
	{
		D3DXVECTOR3 moveVelocity(0.0f, 0.0f, -specData->MoveSpeed);

		//TODO : 충돌체크
		//충돌여부 판단
		// if 정지
		// else 이동

		velocity = moveVelocity;
	}
	else
	{
		velocity = D3DXVECTOR3(0, 0, 0);
		turretAngleSpeed = 0.0f;

		NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
	}
}

void EnemyTank::OnAiAttack(AiState* state)
{
	if (state->IsActive())
	{
		velocity = D3DXVECTOR3(0, 0, 0);
		//TODO : 재장전 체크
		// if - 재장전 처리

		//TODO : 발사 가능 체크

		//TODO : 공격
		ActionFire();

		turretAngleSpeed = 0.0f;
	}

	NextAi(AiType::Search, Math::Random(0.0f, 1.0f));
}

void EnemyTank::OnAiTurnLeft(AiState* state)
{
	if (state->IsActive())
	{
		turretAngleSpeed = -specData->TurnAngle;
	}
	NextAi(AiType::Search, 0.01f);// Math::Random(0.0f, 1.0f));
}

void EnemyTank::OnAiTurnRight(AiState* state)
{
	if (state->IsActive() == true)
	{
		turretAngleSpeed = specData->TurnAngle;
	}
	NextAi(AiType::Search, 0.01f);// Math::Random(0.0f, 1.0f));
}
