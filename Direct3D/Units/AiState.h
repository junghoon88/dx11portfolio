#pragma once
#include "stdafx.h"

struct AiState
{
	wstring Name;

	function<void(AiState*)> Starting  = NULL;
	function<void(AiState*)> Finishing = NULL;
	function<void(AiState*)> Updating  = NULL;

	void Update(void)
	{
		if (Updating == NULL) 
			return;

		Updating(this);
	}

	void Start(void)
	{
		if (Starting == NULL) 
			return;

		Starting(this);
	}

	void Finish(void)
	{
		if (Finishing == NULL) 
			return;

		Finishing(this);
	}
};