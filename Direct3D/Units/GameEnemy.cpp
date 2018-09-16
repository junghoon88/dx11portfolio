#include "stdafx.h"
#include "GameEnemy.h"
#include "GamePlayer.h"
#include "GameData.h"
#include "AiContext.h"
#include "AiState.h"
#include "UnitHpBar.h"

GameEnemy::GameEnemy(wstring matmeshFile, ANIMATION_TYPE animType)
	: GameUnit(matmeshFile, animType)
	, actionElapsedTime(0.0f)
	, startAi(AiType::Idle)
	, player(NULL)
{
	//AI ÃÊ±âÈ­
	{
		aiContext = new AiContext();
		
		AiState* state = NULL;
		state = new AiState();
		//state->Starting  = NULL;
		state->Updating  = bind(&GameEnemy::OnIdle, this, placeholders::_1);
		//state->Finishing = NULL;
		aiIdle.first = aiContext->AddState(L"Idle", state);
		aiIdle.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnFollow, this, placeholders::_1);
		aiFollow.first = aiContext->AddState(L"Follow", state);
		aiFollow.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnAttack, this, placeholders::_1);
		aiAttack.first = aiContext->AddState(L"Attack", state);
		aiAttack.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnDamage, this, placeholders::_1);
		aiDamage.first = aiContext->AddState(L"Damage", state);
		aiDamage.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnDeath, this, placeholders::_1);
		aiDeath.first = aiContext->AddState(L"Death", state);
		aiDeath.second = state;
	}
	aiContext->Enable(true);
	aiContext->StartState((UINT)startAi);


	hpBar = new UnitHpBar(this);

}

GameEnemy::~GameEnemy()
{
	SAFE_DELETE(aiContext);
	SAFE_DELETE(hpBar);

}

void GameEnemy::Update(void)
{
	aiContext->Update();
	GameUnit::Update();


	hpBar->Update(unitInfo.HP / unitInfo.HPMax);
}

void GameEnemy::Render(void)
{
	GameUnit::Render();

	hpBar->Render();

}

void GameEnemy::OnIdle(AiState* state)
{
	OnAiIdle(state);
}

void GameEnemy::OnFollow(AiState* state)
{
	OnAiFollow(state);
}

void GameEnemy::OnAttack(AiState* state)
{
	OnAiAttack(state);
}

void GameEnemy::OnDamage(AiState * state)
{
	OnAiDamage(state);
}

void GameEnemy::OnDeath(AiState * state)
{
	OnAiDeath(state);
}

void GameEnemy::SetStartAi(AiType type)
{
	startAi = type;

	UINT index = (UINT)-1;
	switch (type)
	{
	case AiType::Idle:		index = aiIdle.first;		break;
	case AiType::Follow:	index = aiFollow.first;		break;
	case AiType::Attack:	index = aiAttack.first;		break;
	case AiType::Damage:	index = aiDamage.first;		break;
	case AiType::Death:		index = aiDeath.first;		break;
	default: assert(false);
	}

	aiContext->Enable(true);
	aiContext->StartState(index);
}

float GameEnemy::CalcDistancePlayer(void)
{
	D3DXVECTOR3 pPos, ePos;
	player->GetPosition(pPos);
	this->GetPosition(ePos);
	D3DXVECTOR3 vec = pPos - ePos;
	vec.y = 0.0f;
	float dist = D3DXVec3Length(&vec);

	return dist;
}

void GameEnemy::CalcDistancePlayer(D3DXVECTOR3 & dir, float & dist)
{
	D3DXVECTOR3 pPos, ePos;
	player->GetPosition(pPos);
	this->GetPosition(ePos);
	D3DXVECTOR3 vec = pPos - ePos;
	vec.y = 0.0f;
	dist = D3DXVec3Length(&vec);
	
	D3DXVec3Normalize(&dir, &vec);
}
