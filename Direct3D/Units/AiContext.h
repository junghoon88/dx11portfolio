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

	void StartState(UINT index);

private:

	void Play(UINT index);
	void Play(AiState* state);

public:
	inline AiState* GetCurrent(void) { return current; }

private:
	bool bActive;
	bool bEnable;

	AiState* current;

	vector<AiState*> states;
};