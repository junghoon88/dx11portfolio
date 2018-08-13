#include "stdafx.h"
#include "GamePlayer.h"

#include "GamePlayerInput.h"
#include "GameData.h"

#include "../Model/Bounding/BoundingBox.h"
#include "../Model/Bounding/Ray.h"
#include "../Model/Bounding/ModelShape.h"
#include "PlayerBullet.h"
#include "../Executes/DrawModel.h"

GamePlayer::GamePlayer(wstring path)
	: GameUnit(path)
	, drawModel(NULL)
{
	LoadAnimation(path);


	specData = new GamePlayerSpec();
	input = new GamePlayerInput();

	boneWaist = model->GetBone(L"Spine");
	assert(boneWaist != NULL);
	boneWaistSrc = boneWaist->GetTransform();

	moveDirection = D3DXVECTOR3(0, 0, 0);
	bRun = true;
	bActiveBooster = false;
	rootRotationAngle = 0.0f;
	rootElapsedAngle = 0.0f;
	curWaistAngle = 0.0f;

	bOverwriteLowerAction = false;
	bOverwriteUpperAction = false;
	actionElapsedTime = 0.0f;
	moveElapsedTime = 0.0f;

	attackDelay = 0.3f;
	attackTime = 0.0f;
	attackNum = 1;

	box = new BoundingBox(min, max);

	PlayLowerAction(LowerAction::Idle);
	PlayUpperAction(UpperAction::Idle);
}

GamePlayer::~GamePlayer()
{
	SAFE_DELETE(box);

	SAFE_DELETE(specData);
	SAFE_DELETE(input);
}

void GamePlayer::Update(ModelShape* modelShape)
{
	HandleInput();

	if (prepareLowerAction != LowerAction::Unknown
		|| bOverwriteLowerAction == true)
	{
		PlayLowerAction(prepareLowerAction);
	}

	if (prepareUpperAction != UpperAction::Unknown
		|| bOverwriteUpperAction == true)
	{
		PlayUpperAction(prepareUpperAction);
	}


	GameUnit::Update(modelShape);

	box->SetSocketTransform(Transformed());
	modelShape->AddBoundingBox(box, MathDX::Aqua);

	D3DXVECTOR3 pos, dir;
	GetPosition(pos);
	GetDirection(dir);
	Ray ray(pos, dir * 20.0f);
	modelShape->AddLine(ray.Position, ray.Position + ray.Direction, MathDX::Yellow);

	ActionRotation();
}

void GamePlayer::Render(void)
{
	GameUnit::Render();
}

void GamePlayer::PostRender(void)
{
	if (ImGui::Begin("GamePlayer"))
	{
		ImGui::InputFloat("curWaistAngle", &curWaistAngle);

		ImGui::InputInt("BulletType", &attackNum);
		attackNum = Math::Clamp(attackNum, 0, 1);
		//model->ShowTreeNode(model->GetBone(0));

		ImGui::End();
	}

}

void GamePlayer::LoadAnimation(wstring path)
{
	UINT lowerAnimCount = (UINT)LowerAction::Count;
	UINT upperAnimCount = (UINT)UpperAction::Count;

	indexLowerAnimations.assign(lowerAnimCount, UINT(0));
	indexUpperAnimations.assign(upperAnimCount, UINT(0));

	//Lower Animation
	{
		indexLowerAnimations[(UINT)LowerAction::Idle] = AddClip(path + L"Low_Idle.anim");
		indexLowerAnimations[(UINT)LowerAction::Run] = AddClip(path + L"Low_Run.anim");
		indexLowerAnimations[(UINT)LowerAction::Damage] = AddClip(path + L"Low_Damage.anim");
		indexLowerAnimations[(UINT)LowerAction::Walk] = AddClip(path + L"Low_Walk.anim");
		indexLowerAnimations[(UINT)LowerAction::LeftTurn] = AddClip(path + L"Low_TurnLeft.anim");
		indexLowerAnimations[(UINT)LowerAction::RightTurn] = AddClip(path + L"Low_TurnRight.anim");
		indexLowerAnimations[(UINT)LowerAction::BackWalk] = AddClip(path + L"Low_WalkBack.anim");
		indexLowerAnimations[(UINT)LowerAction::ForwardDead] = AddClip(path + L"Low_DeathFront.anim");
		indexLowerAnimations[(UINT)LowerAction::BackwardDead] = AddClip(path + L"Low_DeathBack.anim");
		indexLowerAnimations[(UINT)LowerAction::LeftDead] = AddClip(path + L"Low_DeathLeft.anim");
		indexLowerAnimations[(UINT)LowerAction::RightDead] = AddClip(path + L"Low_DeathRight.anim");
						   
		indexLowerAnimations[(UINT)LowerAction::BoosterPrepare] = AddClip(path + L"Low_BoosterStart.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterActive] = AddClip(path + L"Low_BoosterMove.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterFinish] = AddClip(path + L"Low_BoosterEnd.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterBreak] = AddClip(path + L"Low_BoosterMiss.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterLeftTurn] = AddClip(path + L"Low_BoosterTurnLeft.anim");
		indexLowerAnimations[(UINT)LowerAction::BoosterRightTurn] = AddClip(path + L"Low_BoosterTurnRight.anim");
	}

	//Upper Animation
	{
		indexUpperAnimations[(UINT)UpperAction::Idle] = AddClip(path + L"Up_Idle.anim");
		indexUpperAnimations[(UINT)UpperAction::Run] = AddClip(path + L"Up_Run.anim");
		indexUpperAnimations[(UINT)UpperAction::Damage] = AddClip(path + L"Up_Damage.anim");
		indexUpperAnimations[(UINT)UpperAction::WeaponChange] = AddClip(path + L"Up_Change.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardNonFire] = AddClip(path + L"Up_NonAttackFront.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftNonFire] = AddClip(path + L"Up_NonAttackLeft.anim");
		indexUpperAnimations[(UINT)UpperAction::RightNonFire] = AddClip(path + L"Up_NonAttackRight.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardMachineGunFire] = AddClip(path + L"Up_AttackFrontBase.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftMachineGunFire] = AddClip(path + L"Up_AttackLeftBase.anim");
		indexUpperAnimations[(UINT)UpperAction::RightMachineGunFire] = AddClip(path + L"Up_AttackRightBase.anim");
		indexUpperAnimations[(UINT)UpperAction::ReloadMachineGun] = AddClip(path + L"Up_ReloadBase.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardShotgunFire] = AddClip(path + L"Up_AttackFrontShotgun.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftShotgunFire] = AddClip(path + L"Up_AttackLeftShotgun.anim");
		indexUpperAnimations[(UINT)UpperAction::RightShotgunFire] = AddClip(path + L"Up_AttackRightShotgun.anim");
		indexUpperAnimations[(UINT)UpperAction::ReloadShotgun] = AddClip(path + L"Up_ReloadShotgun.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardHandgunFire] = AddClip(path + L"Up_AttackFrontHandgun.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftHandgunFire] = AddClip(path + L"Up_AttackLeftHandgun.anim");
		indexUpperAnimations[(UINT)UpperAction::RightHandgunFire] = AddClip(path + L"Up_AttackRightHandgun.anim");
		indexUpperAnimations[(UINT)UpperAction::ReloadHandgun] = AddClip(path + L"Up_ReloadHandgun.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::ForwardDead] = AddClip(path + L"Up_DeathFront.anim");
		indexUpperAnimations[(UINT)UpperAction::BackwardDead] = AddClip(path + L"Up_DeathBack.anim");
		indexUpperAnimations[(UINT)UpperAction::LeftDead] = AddClip(path + L"Up_DeathLeft.anim");
		indexUpperAnimations[(UINT)UpperAction::RightDead] = AddClip(path + L"Up_DeathRight.anim");
						   
		indexUpperAnimations[(UINT)UpperAction::BoosterPrepare] = AddClip(path + L"Up_BoosterStart.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterActive] = AddClip(path + L"Up_BoosterMove.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterFinish] = AddClip(path + L"Up_BoosterEnd.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterBreak] = AddClip(path + L"Up_BoosterMiss.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterLeftTurn] = AddClip(path + L"Up_BoosterTurnLeft.anim");
		indexUpperAnimations[(UINT)UpperAction::BoosterRightTurn] = AddClip(path + L"Up_BoosterTurnRight.anim");
	}

	runDurationTime			 = GetClip(indexLowerAnimations[(UINT)LowerAction::Run])->Duration;
	walkDurationTime		 = GetClip(indexLowerAnimations[(UINT)LowerAction::Walk])->Duration;
	boosterFinishDurationTim = GetClip(indexLowerAnimations[(UINT)LowerAction::BoosterFinish])->Duration;
	boosterBreakDurationTime = GetClip(indexLowerAnimations[(UINT)LowerAction::BoosterBreak])->Duration;
	weaponChangeDurationTime = GetClip(indexUpperAnimations[(UINT)UpperAction::WeaponChange])->Duration;
}

void GamePlayer::HandleInput(void)
{
	HandleMove();
	HandleRotation();
	HandleAttack();
}

void GamePlayer::HandleMove(void)
{
	bool goingOn = false;
	D3DXVECTOR3 velocity(0, 0, 0);

	if (bActiveBooster)
	{

	}
	else
	{
		moveDirection = D3DXVECTOR3(0, 0, 0);

		if (input->IsPressMovement())
		{
			float moveSpeed = 0.0f;

			if (bRun)
			{
				moveSpeed = specData->RunSpeed;
			}
			else
			{
				moveSpeed = specData->WalkSpeed;
			}


			if (input->Pressed(GamePlayerKey::MoveForward))
				moveDirection += D3DXVECTOR3(0, 0, 1);
			if (input->Pressed(GamePlayerKey::MoveBackward))
				moveDirection += D3DXVECTOR3(0, 0, -1);
			if (input->Pressed(GamePlayerKey::MoveLeft))
				moveDirection += D3DXVECTOR3(-1, 0, 0);
			if (input->Pressed(GamePlayerKey::MoveRight))
				moveDirection += D3DXVECTOR3(1, 0, 0);


			if (moveDirection != D3DXVECTOR3(0, 0, 0))
			{
				D3DXVec3Normalize(&moveDirection, &moveDirection);
				//velocity = moveDirection * moveSpeed;
				velocity = moveDirection * moveSpeed * 0.5f;

				goingOn = true;
			}
		}//if (input->IsPressMovement())

		ActionMovement(moveDirection);
	}//else 

	if (goingOn)
	{
		this->GameModel::velocity = velocity;
	}
	else
	{
		this->GameModel::velocity = D3DXVECTOR3(0, 0, 0);
	}

}

void GamePlayer::HandleRotation(void)
{
	if (input->IsPressTurn())
	{
		float angle = specData->TurnAngle; //회전속도
		D3DXVECTOR2 dir(0, 0);

		if (input->Pressed(GamePlayerKey::TurnLeft))
			dir.x += -angle;
		if (input->Pressed(GamePlayerKey::TurnRight))
			dir.x += angle;

		if (D3DXVec2Length(&dir) > EPSILON6)
		{
			Rotate(dir);

			if (dir.x < 0)
			{
				bool b = false;
				b |= curLowerAction == LowerAction::Idle;
				b |= curLowerAction == LowerAction::RightTurn;
	
				if (b == true)
					prepareLowerAction = LowerAction::LeftTurn;
			}
			else
			{
				bool b = false;
				b |= curLowerAction == LowerAction::Idle;
				b |= curLowerAction == LowerAction::RightTurn;
	
				if (b == true)
					prepareLowerAction = LowerAction::RightTurn;
			}
		}
	}//if (input->IsPressTurn)
	else
	{
		bool b = false;
		b |= curLowerAction == LowerAction::LeftTurn;
		b |= curLowerAction == LowerAction::RightTurn;

		if (b == true)
			prepareLowerAction = LowerAction::Idle;
	}
}

void GamePlayer::HandleAttack(void)
{
	if (attackTime > 0.0f)
	{
		attackTime -= Time::Delta();
		return;
	}

	if (gMouse->IsDown(MOUSE_LBUTTON))
	{
		D3DXVECTOR3 pos, dir;

		//D3DXMATRIX transformed = Transformed();
		GetPosition(pos);
		GetDirection(dir);

		D3DXMATRIX mat = model->GetBone(14)->GetTransform();
		//D3DXMATRIX mat = model->GetBone(24)->GetTransform();
		D3DXVec3TransformCoord(&pos, &pos, &mat);
		pos.y += 3.0f;


		PlayerBullet* bullet = NULL;
		
		if (attackNum == 0)
		{
			//bullet = new PlayerBullet(PlayerBullet::BulletType::Straight, pos, dir);
		}
		else if (attackNum == 1)
		{
			dir.y = 1.0f;
			//bullet = new PlayerBullet(PlayerBullet::BulletType::Parabola, pos, dir);
		}

		//if (bullet != NULL)
		//	drawModel->AddPlayerBullet(bullet);

		attackTime = attackDelay;
	}
}

void GamePlayer::ActionRotation(void)
{
	if (rootRotationAngle > curWaistAngle)
		rootElapsedAngle = Math::Clamp(3.0f, 0.0f, rootRotationAngle - curWaistAngle);
	else if (rootRotationAngle < curWaistAngle)
		rootElapsedAngle = Math::Clamp(-3.0f, rootRotationAngle - curWaistAngle, 0.0f);
	else
		rootElapsedAngle = 0.0f;

	 

#if 0
	{
		bool bRes = false;
		bRes |= curUpperAction == UpperAction::ForwardMachineGunFire;
		bRes |= curUpperAction == UpperAction::ForwardShotgunFire;
		bRes |= curUpperAction == UpperAction::ForwardHandgunFire;
		bRes |= curUpperAction == UpperAction::ForwardNonFire;
		bRes |= curUpperAction == UpperAction::LeftMachineGunFire;
		bRes |= curUpperAction == UpperAction::LeftShotgunFire;
		bRes |= curUpperAction == UpperAction::LeftHandgunFire;
		bRes |= curUpperAction == UpperAction::LeftNonFire;
		bRes |= curUpperAction == UpperAction::RightMachineGunFire;
		bRes |= curUpperAction == UpperAction::RightShotgunFire;
		bRes |= curUpperAction == UpperAction::RightHandgunFire;
		bRes |= curUpperAction == UpperAction::RightNonFire;
		if (bRes)
		{
			curWaistAngle = -rootElapsedAngle;
		}
	}
#endif

	bool bRes = false;
	bRes |= curLowerAction == LowerAction::Idle;
	bRes |= curLowerAction == LowerAction::Damage;
	bRes |= curLowerAction == LowerAction::ForwardDead;
	bRes |= curLowerAction == LowerAction::BackwardDead;
	bRes |= curLowerAction == LowerAction::LeftDead;
	bRes |= curLowerAction == LowerAction::RightDead;
	if (bRes)
	{
		rootRotationAngle = curWaistAngle;
	}
	else
	{
		curWaistAngle += rootElapsedAngle;

		//Y축 회전 
		D3DXMATRIX RY;
		D3DXMatrixRotationY(&RY, Math::ToRadian(curWaistAngle));
		SetRootAxis(RY);

#if 0
		//오른쪽 축을 기준으로 회전 -> Spine 의 right 축이 World의 Y축과 같게 상위에서 변환되어 있음
		//-> Spine 의 하위 노드들을 다시 회전시킨다. (상체가 해당됨)
		D3DXVECTOR3 right;
		right.x = boneWaist->GetTransform()._11;
		right.y = boneWaist->GetTransform()._12;
		right.z = boneWaist->GetTransform()._13;
		D3DXMATRIX RA;
		D3DXMatrixRotationAxis(&RA, &right, Math::ToRadian(curWaistAngle * -0.25f));
		D3DXMATRIX temp = boneWaist->GetTransform();
		boneWaist->SetTransform(temp * RA);
#endif
	}

	//회전 트랜스폼 업데이트
	D3DXMATRIX transformed = Transformed();
	model->CopyAbsoluteBoneTo(transformed, boneTransforms);
}

void GamePlayer::PlayLowerAction(LowerAction action)
{
	PlayLowerAction(action, 0.0f);
}
void GamePlayer::PlayLowerAction(LowerAction action, float startTime)
{
	AnimationPlayMode mode = AnimationPlayMode::Repeat;
	float blendTime = 0.0f;

	switch (action)
	{
		case LowerAction::Idle:
			blendTime = 0.5f;
		break;

		case LowerAction::Run:
			blendTime = 0.3f;
		break;

		case LowerAction::Damage:
			mode = AnimationPlayMode::Once;
			blendTime = 0.2f;
		break;

		case LowerAction::Walk:
			blendTime = 0.3f;
		break;

		case LowerAction::BackWalk:
			blendTime = 0.3f;
		break;

		case LowerAction::LeftTurn:
		case LowerAction::RightTurn:
			blendTime = 0.5f;
		break;

		case LowerAction::ForwardDead:
		case LowerAction::BackwardDead:
		case LowerAction::LeftDead:
		case LowerAction::RightDead:
			mode = AnimationPlayMode::Once;
			blendTime = 0.2f;
		break;

		case LowerAction::BoosterPrepare:
		case LowerAction::BoosterActive:
		case LowerAction::BoosterFinish:
		case LowerAction::BoosterBreak:
		case LowerAction::BoosterLeftTurn:
		case LowerAction::BoosterRightTurn:
			mode = AnimationPlayMode::Once;
			blendTime = 0.2f;
		break;

		default: assert(false); break;
	}

	PlayAnim(indexLowerAnimations[(UINT)action], startTime, blendTime, 1.0f, mode);
	curLowerAction = action;
	prepareLowerAction = LowerAction::Unknown;
	bOverwriteLowerAction = false;
}

void GamePlayer::PlayUpperAction(UpperAction action)
{
	PlayUpperAction(action, 0.0f);
}
void GamePlayer::PlayUpperAction(UpperAction action, float startTime)
{
	AnimationPlayMode mode = AnimationPlayMode::Repeat;
	float blendTime = 0.0f;

	switch (action)
	{
		case UpperAction::Idle:
			blendTime = 0.5f;
		break;

		case UpperAction::Run:
			blendTime = 0.3f;
		break;
		
		case UpperAction::Damage:
			mode = AnimationPlayMode::Once;
			blendTime = 0.2f;
		break;
		
		case UpperAction::WeaponChange:
			mode = AnimationPlayMode::Once;
			blendTime = 0.2f;
		break;
		
		case UpperAction::ForwardNonFire:
		case UpperAction::LeftNonFire:
		case UpperAction::RightNonFire:
			mode = AnimationPlayMode::Once;
			blendTime = 0.5f;
		break;
		
		case UpperAction::ForwardMachineGunFire:
		case UpperAction::LeftMachineGunFire:
		case UpperAction::RightMachineGunFire:
		case UpperAction::ForwardShotgunFire:
		case UpperAction::LeftShotgunFire:
		case UpperAction::RightShotgunFire:
		case UpperAction::ForwardHandgunFire:
		case UpperAction::LeftHandgunFire:
		case UpperAction::RightHandgunFire:
			mode = AnimationPlayMode::Once;
			blendTime = 0.5f;
		break;
		
		case UpperAction::ReloadMachineGun:
		case UpperAction::ReloadShotgun:
		case UpperAction::ReloadHandgun:
			mode = AnimationPlayMode::Once;
			blendTime = 0.5f;
		break;

		case UpperAction::ForwardDead:
		case UpperAction::BackwardDead:
		case UpperAction::LeftDead:
		case UpperAction::RightDead:
			mode = AnimationPlayMode::Once;
			blendTime = 0.2f;
		break;

		case UpperAction::BoosterPrepare:
		case UpperAction::BoosterActive:
		case UpperAction::BoosterFinish:
		case UpperAction::BoosterBreak:
		case UpperAction::BoosterLeftTurn:
		case UpperAction::BoosterRightTurn:
			mode = AnimationPlayMode::Once;
			blendTime = 0.2f;
		break;

		default: assert(false); break;
	}

	PlayAnim(indexUpperAnimations[(UINT)action], startTime, blendTime, 1.0f, mode);
	curUpperAction = action;
	prepareUpperAction = UpperAction::Unknown;
	bOverwriteUpperAction = false;
}

void GamePlayer::ActionMovement(D3DXVECTOR3 direction)
{
	bool bMove = true;

	if (direction.z > 0.0f)
	{
		if (direction.x < 0.0f)
			rootRotationAngle = -45.0f;
		else if (direction.x > 0.0f)
			rootRotationAngle = 45.0f;
		else 
			rootRotationAngle = 0.0f;

		if (bRun)
		{
			if (curLowerAction != LowerAction::Run)
				prepareLowerAction = LowerAction::Run;
		}
		else
		{
			if (curLowerAction != LowerAction::Walk)
				prepareLowerAction = LowerAction::Walk;
		}
	}
	else if (direction.z < 0.0f)
	{
		if (direction.x < 0.0f)
			rootRotationAngle = -30.0f;
		else if (direction.x > 0.0f)
			rootRotationAngle = 30.0f;
		else
			rootRotationAngle = 0.0f;

		if (curLowerAction != LowerAction::BackWalk)
			prepareLowerAction = LowerAction::BackWalk;
	}
	else
	{
		if (direction.x != 0.0f)
		{
			if (direction.x < 0.0f)
				rootRotationAngle = -90.0f;
			else if(direction.x > 0.0f)
				rootRotationAngle = 90.0f;

			if (bRun)
			{
				if (curLowerAction != LowerAction::Run)
					prepareLowerAction = LowerAction::Run;
			}
			else
			{
				if (curLowerAction != LowerAction::Walk)
					prepareLowerAction = LowerAction::Walk;
			}
		}
		else
		{
			bMove = false;

			//rootRotationAngle = 0.0f;
			moveElapsedTime = 0.0f;

			bool b = false;
			b |= curLowerAction == LowerAction::Run;
			b |= curLowerAction == LowerAction::Walk;
			b |= curLowerAction == LowerAction::BackWalk;
			if (b)
				prepareLowerAction = LowerAction::Idle;
		}
	}
}

