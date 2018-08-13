#include "stdafx.h"
#include "AiContext.h"

AiContext::AiContext()
	: bActive(true), bEnable(true)
	, active(NULL), next(NULL)
{
}

AiContext::~AiContext()
{
}

void AiContext::Update(void)
{
	if (bActive == false)
		return;
	if (bEnable == false)
		return;
	if (active == NULL && next == NULL)
		return;

	if (active == NULL)
	{
		Play(next, next->ActiveTime);
	}
	else if (next != NULL && active->IsActive() == false)//next->ActiveTime == 0.0f)
	{
		Play(next, next->ActiveTime);

		next = NULL;
	}
	else
	{
		active->Update();
		if (active->IsActive())
		{
			active->ActiveTime -= Time::Delta();

			if (active->ActiveTime < 0.0f)
				active->ActiveTime = 0.0f;
		}
	}
}

AiState* AiContext::FindState(UINT index)
{
	if (bActive == false)
		return NULL;
	
	assert(index < states.size());
	return states[index];
}

UINT AiContext::AddState(wstring name, AiState* state)
{
	if (bActive == false)
		return UINT(-1);

	state->Name = name;
	states.push_back(state);

	return (states.size() - 1);
}

void AiContext::RemoveState(UINT index)
{
	assert(index < states.size());
	states.erase(states.begin() + index);
}

void AiContext::RemoveStateAll(void)
{
	states.clear();
}

void AiContext::NextState(UINT index, float activeTime)
{
	if (bActive == false)
		return;

	next = FindState(index);
	next->ActiveTime = activeTime;
}

void AiContext::StartState(UINT index, float activeTime)
{
	bActive = true;

	active = NULL;
	next = NULL;

	Play(index, activeTime);
}

void AiContext::Play(UINT index, float activeTime)
{
	if (bActive == false)
		return;

	Play(FindState(index), activeTime);
}

void AiContext::Play(AiState* state, float activeTime)
{
	if (bActive == false)
		return;
	
	if (active != NULL)
		active->Finish();

	active = state;
	active->ActiveTime = activeTime;
	

	active->Start();
}