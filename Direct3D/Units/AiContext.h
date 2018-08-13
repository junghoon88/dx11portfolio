#pragma once

#include "AiState.h"

class AiContext
{
public:
	AiContext();
	~AiContext();

	void Enable(bool val) { bEnable = val; }

	void Update(void);
	AiState* FindState(UINT index);
	UINT AddState(wstring name, AiState* state);
	void RemoveState(UINT index);
	void RemoveStateAll(void);

	void NextState(UINT index, float activeTime);
	void StartState(UINT index, float activeTime);

private:

	void Play(UINT index, float activeTime);
	void Play(AiState* state, float activeTime);

public:
	inline AiState* GetActive(void) { return active; }
	inline AiState* GetNext(void) { return next; }

private:
	bool bActive;
	bool bEnable;

	AiState* active;
	AiState* next;

	vector<AiState*> states;
};