#include "stdafx.h"
#include "AiContext.h"

AiContext::AiContext()
	: bActive(true), bEnable(true)
	, current(NULL)
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
	if (current == NULL)
		return;

	current->Update();
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

void AiContext::StartState(UINT index)
{
	bActive = true;

	current = NULL;

	Play(index);
}

void AiContext::Play(UINT index)
{
	if (bActive == false)
		return;

	Play(FindState(index));
}

void AiContext::Play(AiState* state)
{
	if (bActive == false)
		return;
	
	if (current != NULL)
		current->Finish();

	current = state;
	current->Start();
}