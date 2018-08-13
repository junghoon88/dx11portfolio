#include "stdafx.h"
#include "Keyboard.h"

Keyboard* Keyboard::instance = NULL;


Keyboard::Keyboard()
{
	ZeroMemory(&keyState,	 sizeof(byte) * MAX_INPUT_KEY);
	ZeroMemory(&keyOldState, sizeof(byte) * MAX_INPUT_KEY);
	ZeroMemory(&keyMap,		 sizeof(byte) * MAX_INPUT_KEY);
}


Keyboard::~Keyboard()
{
}

void Keyboard::Update(void)
{
	memcpy(keyOldState, keyState, sizeof(keyOldState));

	ZeroMemory(keyState, sizeof(keyState));
	ZeroMemory(keyMap, sizeof(keyMap));

	GetKeyboardState(keyState);

	for (DWORD i = 0; i < MAX_INPUT_KEY; i++)
	{
		keyState[i] = keyState[i] & 0x80 ? 1 : 0;

		int oldState = keyOldState[i];
		int state = keyState[i];

		if (oldState == 0 && state == 1)
			keyMap[i] = KEY_INPUT_STATUS_DOWN;
		else if (oldState == 1 && state == 0)
			keyMap[i] = KEY_INPUT_STATUS_UP;
		else if (oldState == 1 && state == 1)
			keyMap[i] = KEY_INPUT_STATUS_PRESS;
		else 
			keyMap[i] = KEY_INPUT_STATUS_NONE;
	}
}
