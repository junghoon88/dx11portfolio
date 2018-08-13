#pragma once
#include "stdafx.h"

enum class GamePlayerKey
{
	MoveForward = 0, MoveBackward, MoveLeft, MoveRight,
	TurnLeft, TurnRight,
	WeaponFire, WeaponReload, WeaponChange,
	Boost,

	Count
};

struct GamePlayerInput
{
private:
	DWORD keyboard[(UINT)GamePlayerKey::Count];

public:
	GamePlayerInput()
	{
		SetGameKey(GamePlayerKey::MoveForward,	'W');
		SetGameKey(GamePlayerKey::MoveBackward, 'S');
		SetGameKey(GamePlayerKey::MoveLeft,		'A');
		SetGameKey(GamePlayerKey::MoveRight,	'D');

		SetGameKey(GamePlayerKey::TurnLeft,		'F');
		SetGameKey(GamePlayerKey::TurnRight,	'H');

		SetGameKey(GamePlayerKey::WeaponFire,	'G');
		SetGameKey(GamePlayerKey::WeaponReload, 'T');
		SetGameKey(GamePlayerKey::WeaponChange, 'V');
		SetGameKey(GamePlayerKey::Boost,		VK_SPACE);
	}


	void SetGameKey(GamePlayerKey key, DWORD setKey)
	{
		keyboard[(UINT)key] = setKey;
	}

	bool Pressed(GamePlayerKey key)
	{
		bool res = true;
		res &= (gMouse->IsPress(MOUSE_RBUTTON) == false);
		res &= gKeyboard->IsPress(keyboard[(UINT)key]);
		return res;
	}

	bool Released(GamePlayerKey key)
	{
		bool res = true;
		res &= (gMouse->IsPress(MOUSE_RBUTTON) == false);
		res &= gKeyboard->IsUp(keyboard[(UINT)key]);
		return res;
	}

	bool Stroke(GamePlayerKey key)
	{
		bool res = true;
		res &= (gMouse->IsPress(MOUSE_RBUTTON) == false);
		res &= gKeyboard->IsDown(keyboard[(UINT)key]);
		return res;
	}

	bool IsPressTurn()
	{
		bool res = false;
		res |= Pressed(GamePlayerKey::TurnLeft);
		res |= Pressed(GamePlayerKey::TurnRight);
		return res;
	}

	bool IsPressMovement()
	{
		bool res = false;
		res |= Pressed(GamePlayerKey::MoveForward);
		res |= Pressed(GamePlayerKey::MoveBackward);
		res |= Pressed(GamePlayerKey::MoveLeft);
		res |= Pressed(GamePlayerKey::MoveRight);
		return res;
	}
};