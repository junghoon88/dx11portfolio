#include "stdafx.h"
#include "GamePlayer.h"

#include "GamePlayerInput.h"
#include "GameData.h"

#include "../Model/Bounding/BoundingBox.h"
#include "../Model/Bounding/Ray.h"
#include "PlayerBullet.h"

GamePlayer::GamePlayer(wstring path, ANIMATION_TYPE animType)
	: GameUnit(path, animType)
	, moveDirection(D3DXVECTOR3(0, 0, 0))
	, bRun(false)
	, attackTime(0.0f)
{
	specData = new GamePlayerSpec();
	input = new GamePlayerInput();

}

GamePlayer::~GamePlayer()
{
	SAFE_DELETE(specData);
	SAFE_DELETE(input);
}

void GamePlayer::Init(void)
{
	LinkAnimation();

	PlayAction(PlayerAction::Idle);
}

void GamePlayer::Update(void)
{
	HandleInput();
	
	//if (bOverwriteAction == true)
	if (curAction != nextAction)
	{
		PlayAction(nextAction);
	}


	GameUnit::Update();

	if (GameModel::isPlayEnd)
	{
		SetPlayerIdle();
	}
}

void GamePlayer::Render(void)
{
	GameUnit::Render();
}

void GamePlayer::PostRender(void)
{
	ImGui::Begin("Player Anim Test - Current");
	{
		AnimationBinder* binder = blenders[0]->Current;
		{
			//ImGui::Text("Current");
			ImGui::Text(String::ToString(binder->clipName).c_str());
			ImGui::Text(String::SInt("PlayerMode - %d", (int)binder->PlayMode).c_str());
			ImGui::InputFloat("PlayTime", &binder->PlayTime);
			ImGui::InputFloat("Speed", &binder->Speed);
		}
	}
	ImGui::End();
	ImGui::Begin("Player Anim Test - Next");
	{
		AnimationBinder* binder = blenders[0]->Next;
		{
			//ImGui::Text("Next");
			ImGui::Text(String::ToString(binder->clipName).c_str());
			ImGui::Text(String::SInt("PlayerMode - %d", (int)binder->PlayMode).c_str());
			ImGui::InputFloat("PlayTime", &binder->PlayTime);
			ImGui::InputFloat("Speed", &binder->Speed);
		}
	}
	ImGui::End();
}

void GamePlayer::LinkAnimation(void)
{
	UINT animCount = (UINT)PlayerAction::Count;

	indexAnimations.assign(animCount, UINT(-1));

	// Animation
	{
		indexAnimations[(UINT)PlayerAction::Idle]				= FindClip(L"idle");		
		indexAnimations[(UINT)PlayerAction::Walk_Forward] 		= FindClip(L"walk-forward");
		indexAnimations[(UINT)PlayerAction::Walk_Backward] 		= FindClip(L"walk-backward");
		indexAnimations[(UINT)PlayerAction::Strafe_Left] 		= FindClip(L"strafe-left");
		indexAnimations[(UINT)PlayerAction::Strafe_Right] 		= FindClip(L"strafe-right");
		indexAnimations[(UINT)PlayerAction::Run]				= FindClip(L"run");
		indexAnimations[(UINT)PlayerAction::Slash] 				= FindClip(L"slash");
		indexAnimations[(UINT)PlayerAction::Slash_3combo] 		= FindClip(L"slash-3combo");
		indexAnimations[(UINT)PlayerAction::Kick] 				= FindClip(L"kick");
		indexAnimations[(UINT)PlayerAction::Charge_Attack] 		= FindClip(L"charge attack");
		indexAnimations[(UINT)PlayerAction::High_Spin_Attack]	= FindClip(L"high spin attack");
		indexAnimations[(UINT)PlayerAction::Power_Up]			= FindClip(L"power up");
		indexAnimations[(UINT)PlayerAction::Spell_Cast]			= FindClip(L"spell cast");
		indexAnimations[(UINT)PlayerAction::Death1] 			= FindClip(L"death1");
		indexAnimations[(UINT)PlayerAction::Death2]				= FindClip(L"death2");
	}

	for (UINT i = 0; i < animCount; i++)
	{
		assert(indexAnimations[i] != UINT(-1));
	}

	//runDurationTime			 = GetClip(indexAnimations[(UINT)PlayerAction::Run])->Duration;
	//walkDurationTime		 = GetClip(indexAnimations[(UINT)PlayerAction::Walk])->Duration;
	//weaponChangeDurationTime = GetClip(indexAnimations[(UINT)PlayerAction::WeaponChange])->Duration;
}

void GamePlayer::HandleInput(void)
{
	HandleMove();
	HandleRotation();
	HandleAttack();
}

void GamePlayer::HandleMove(void)
{
	D3DXVECTOR3 velocity(0, 0, 0);
	bRun = false;

	bool notMove = false;
	notMove |= (curAction == PlayerAction::Slash);
	notMove |= (curAction == PlayerAction::Slash_3combo);
	notMove |= (curAction == PlayerAction::Kick);
	notMove |= (curAction == PlayerAction::Charge_Attack);
	notMove |= (curAction == PlayerAction::High_Spin_Attack);
	notMove |= (curAction == PlayerAction::Power_Up);
	notMove |= (curAction == PlayerAction::Spell_Cast);
	notMove |= (curAction == PlayerAction::Death1);
	notMove |= (curAction == PlayerAction::Death2);

	if (notMove)
	{
		//nextAction = curAction;
	}
	else
	{
		moveDirection = D3DXVECTOR3(0, 0, 0);

		if (input->IsPressMovement())
		{
			if (input->Pressed(GamePlayerKey::MoveForward))		moveDirection += D3DXVECTOR3( 0,  0,  1);
			if (input->Pressed(GamePlayerKey::MoveBackward))	moveDirection += D3DXVECTOR3( 0,  0, -1);
			if (input->Pressed(GamePlayerKey::MoveLeft))		moveDirection += D3DXVECTOR3(-1,  0,  0);
			if (input->Pressed(GamePlayerKey::MoveRight))		moveDirection += D3DXVECTOR3( 1,  0,  0);


			if (moveDirection != D3DXVECTOR3(0, 0, 0))
			{
				D3DXVec3Normalize(&moveDirection, &moveDirection);
			}
		}
		ActionMovement(moveDirection, velocity);
	}

	this->GameModel::velocity = velocity;
}

void GamePlayer::HandleRotation(void)
{
	if (input->IsPressRotate())
	{
		float amount = 0.0f;
		if (input->Pressed(GamePlayerKey::RotateLeft))
		{
			amount -= 90;
		}
		if (input->Pressed(GamePlayerKey::RotateRight))
		{
			amount += 90;
		}
		if (gMouse->IsPress(MOUSE_RBUTTON))
		{
			float sensitive = 30.0f;
			amount += gMouse->GetMoveValue().x * sensitive;
		}

		GameModel::Rotate(D3DXVECTOR2(amount, 0));
	}
}

void GamePlayer::HandleAttack(void)
{
	if (attackTime > 0.0f)
	{
		attackTime -= Time::Delta();
		return;
	}

	float startTime = 0.0f;
	if (input->Stroke(GamePlayerKey::Slash))
	{
		//PlayAction(PlayerAction::Slash, startTime);
		//attackTime += GetClip(indexAnimations[(UINT)PlayerAction::Slash])->Duration - startTime;
	}
	else if (input->Stroke(GamePlayerKey::Slash_3combo))
	{
		startTime = 0.2f;
		PlayAction(PlayerAction::Slash_3combo, startTime);
		attackTime += GetClip(indexAnimations[(UINT)PlayerAction::Slash_3combo])->Duration - startTime;
	}
	else if (input->Stroke(GamePlayerKey::Kick))
	{
		PlayAction(PlayerAction::Kick, startTime);
		attackTime += GetClip(indexAnimations[(UINT)PlayerAction::Kick])->Duration - startTime;
	}
	else if (input->Stroke(GamePlayerKey::Charge_Attack))
	{
		PlayAction(PlayerAction::Charge_Attack, startTime);
		attackTime += GetClip(indexAnimations[(UINT)PlayerAction::Charge_Attack])->Duration - startTime;
	}
	else if (input->Stroke(GamePlayerKey::High_Spin_Attack))
	{
		PlayAction(PlayerAction::High_Spin_Attack, startTime);
		attackTime += GetClip(indexAnimations[(UINT)PlayerAction::High_Spin_Attack])->Duration - startTime;
	}
	else if (input->Stroke(GamePlayerKey::Power_Up))
	{
		PlayAction(PlayerAction::Power_Up, startTime);
		attackTime += GetClip(indexAnimations[(UINT)PlayerAction::Power_Up])->Duration - startTime;
	}
	else if (input->Stroke(GamePlayerKey::Spell_Cast))
	{
		PlayAction(PlayerAction::Spell_Cast, startTime);
		attackTime += GetClip(indexAnimations[(UINT)PlayerAction::Spell_Cast])->Duration - startTime;
	}
}

void GamePlayer::SetPlayerIdle(void)
{
	PlayAction(PlayerAction::Idle);

	//PlayAnim(indexAnimations[(UINT)PlayerAction::Idle], 0.0f, 0.5f, 1.0f);
	//curAction = PlayerAction::Idle;
	//nextAction = PlayerAction::Idle;
}

void GamePlayer::PlayAction(PlayerAction action)
{
	PlayAction(action, 0.0f);
}
void GamePlayer::PlayAction(PlayerAction action, float startTime)
{
	float blendTime = 0.0f;

	switch (action)
	{
		case PlayerAction::Idle:
		{
			for (Bounding* bounding : GameModel::GetBoundingsRef())
			{
				if (bounding->GetBoundingProp() == BoundingProp::Weapon)
				{
					bounding->SetShow(false);
					bounding->SetEnable(false);
				}
			}
			blendTime = 0.5f;
		}
		break;
	
		case PlayerAction::Strafe_Left:
		case PlayerAction::Strafe_Right:
		case PlayerAction::Walk_Forward:
		case PlayerAction::Walk_Backward:
		case PlayerAction::Run:
			blendTime = 0.3f;
		break;

		case PlayerAction::Slash: 
		case PlayerAction::Slash_3combo: 
		case PlayerAction::Charge_Attack: 
		case PlayerAction::High_Spin_Attack:
			{
				Bounding* sword = GameModel::GetBounding("Sword");
				if (sword != NULL)
				{
					sword->SetShow(true);
					sword->SetEnable(true);
				}
				blendTime = 0.2f;
			}
			break;
		case PlayerAction::Kick:
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
		case PlayerAction::Power_Up:
		case PlayerAction::Spell_Cast:
			blendTime = 0.2f;
		break;

		case PlayerAction::Death1: 
		case PlayerAction::Death2:
			blendTime = 0.0f;
		break;
	}

	PlayAnim(indexAnimations[(UINT)action], startTime, blendTime, 1.0f);
	curAction = action;
	nextAction = action;
}

void GamePlayer::ActionMovement(D3DXVECTOR3 direction, OUT D3DXVECTOR3& velocity)
{
	float moveSpeed = 0.0f;

	if (direction.z > 0.0f)
	{
		if (input->Pressed(GamePlayerKey::Run))
		{
			moveSpeed = specData->RunSpeed;
			nextAction = PlayerAction::Run;
		}
		else
		{
			moveSpeed = specData->WalkSpeed;
			nextAction = PlayerAction::Walk_Forward;
		}
	}
	else if (direction.z < 0.0f)
	{
		moveSpeed = specData->WalkBackwardSpeed;
		nextAction = PlayerAction::Walk_Backward;
	}
	else
	{
		if (direction.x != 0.0f)
		{
			moveSpeed = specData->WalkSpeed;

			if (direction.x < 0.0f)
			{
				nextAction = PlayerAction::Strafe_Left;
			}
			else if (direction.x > 0.0f)
			{
				nextAction = PlayerAction::Strafe_Right;
			}
		}
		else
		{
			nextAction = PlayerAction::Idle;
		}
	}

	velocity = direction * moveSpeed;
}

