#include "stdafx.h"
#include "GameEnemy.h"
#include "GameData.h"
#include "AiContext.h"
#include "AiState.h"

#include "../Model/Bounding/BoundingBox.h"
#include "../Model/Bounding/Ray.h"
#include "../Model/Bounding/ModelShape.h"

GameEnemy::GameEnemy(wstring path)
	: GameUnit(path)
	, unitClass(UnitClassId::Unknown)
	, specData(NULL)
	, actionElapsedTime(0.0f)
	, startAi(AiType::Search), startAiTime(0.0f)
{
	CreateWeapon(L"", L"");

	//AI ÃÊ±âÈ­
	{
		aiContext = new AiContext();

		AiState* state = NULL;
		state = new AiState();
		state->Updating = bind(&GameEnemy::OnSearch, this, placeholders::_1);
		aiSearch.first = aiContext->AddState(L"Search", state);
		aiSearch.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnMove, this, placeholders::_1);
		aiMove.first = aiContext->AddState(L"Move", state);
		aiMove.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnAttack, this, placeholders::_1);
		aiAttack.first = aiContext->AddState(L"Attack", state);
		aiAttack.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnTurnLeft, this, placeholders::_1);
		aiTurnLeft.first = aiContext->AddState(L"TurnLeft", state);
		aiTurnLeft.second = state;

		state = new AiState();
		state->Updating = bind(&GameEnemy::OnTurnRight, this, placeholders::_1);
		aiTurnRight.first = aiContext->AddState(L"TurnRight", state);
		aiTurnRight.second = state;
	}
	aiContext->Enable(true);
	aiContext->StartState((UINT)startAi, startAiTime);

	box = new BoundingBox(min, max);
}

GameEnemy::~GameEnemy()
{
	SAFE_DELETE(box);
	SAFE_DELETE(aiContext);
}

void GameEnemy::Update(ModelShape* modelShape)
{
	aiContext->Update();
	GameUnit::Update(modelShape);
}

void GameEnemy::Render(void)
{
	GameUnit::Render();
}

void GameEnemy::OnSearch(AiState* state)
{
	OnAiSearch(state);
}

void GameEnemy::OnMove(AiState* state)
{
	OnAiMove(state);
}

void GameEnemy::OnAttack(AiState* state)
{
	OnAiAttack(state);
}

void GameEnemy::OnTurnLeft(AiState* state)
{
	OnAiTurnLeft(state);
}

void GameEnemy::OnTurnRight(AiState* state)
{
	OnAiTurnRight(state);
}

void GameEnemy::SetStartAi(AiType type, float time)
{
	startAi = type;
	startAiTime = time;

	UINT index = (UINT)-1;
	switch (type)
	{
	case GameEnemy::AiType::Search:		index = aiSearch.first;		break;
	case GameEnemy::AiType::Move:		index = aiMove.first;		break;
	case GameEnemy::AiType::Attack:		index = aiAttack.first;		break;
	case GameEnemy::AiType::TurnLeft:	index = aiTurnLeft.first;	break;
	case GameEnemy::AiType::TurnRight:	index = aiTurnRight.first;	break;
	default: assert(false);
	}

	aiContext->Enable(true);
	aiContext->StartState(index, time);
}

void GameEnemy::NextAi(AiType type, float time)
{
	UINT index = (UINT)-1;
	switch (type)
	{
	case GameEnemy::AiType::Search:		index = aiSearch.first;		break;
	case GameEnemy::AiType::Move:		index = aiMove.first;		break;
	case GameEnemy::AiType::Attack:		index = aiAttack.first;		break;
	case GameEnemy::AiType::TurnLeft:	index = aiTurnLeft.first;	break;
	case GameEnemy::AiType::TurnRight:	index = aiTurnRight.first;	break;
	default: assert(false);
	}

	aiContext->Enable(true);
	aiContext->NextState(index, time);
}
