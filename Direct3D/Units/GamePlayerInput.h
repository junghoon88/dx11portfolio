#pragma once
#include "stdafx.h"

enum class GamePlayerKey
{
	MoveForward = 0, MoveBackward, MoveLeft, MoveRight, Run,
	RotateLeft, RotateRight,
	Slash, //Mouse
	Slash_3combo, Kick, Charge_Attack, High_Spin_Attack,
	Power_Up,
	Spell_Cast,

	Count
};

struct GamePlayerInput
{
private:
	DWORD keyboard[(UINT)GamePlayerKey::Count];

public:
	GamePlayerInput()
	{
		SetGameKey(GamePlayerKey::MoveForward,		'W');
		SetGameKey(GamePlayerKey::MoveBackward,		'S');
		SetGameKey(GamePlayerKey::MoveLeft,			'A');
		SetGameKey(GamePlayerKey::MoveRight,		'D');
		SetGameKey(GamePlayerKey::Run,				VK_LSHIFT);
		SetGameKey(GamePlayerKey::RotateLeft,		'Q');
		SetGameKey(GamePlayerKey::RotateRight,		'E');

		SetGameKey(GamePlayerKey::Slash,			MOUSE_LBUTTON);

		SetGameKey(GamePlayerKey::Slash_3combo,		'1');
		SetGameKey(GamePlayerKey::Kick,				'F');
		SetGameKey(GamePlayerKey::Charge_Attack,	'2');
		SetGameKey(GamePlayerKey::High_Spin_Attack, '3');

		SetGameKey(GamePlayerKey::Power_Up,			'4');
		SetGameKey(GamePlayerKey::Spell_Cast,		'5');
	}


	void SetGameKey(GamePlayerKey key, DWORD setKey)
	{
		keyboard[(UINT)key] = setKey;
	}

	bool Pressed(GamePlayerKey key)
	{
		if (key == GamePlayerKey::Slash)
		{
			return gMouse->IsPress(keyboard[(UINT)key]);
		}
		else
		{
			return gKeyboard->IsPress(keyboard[(UINT)key]);
		}
	}

	bool Released(GamePlayerKey key)
	{
		if (key == GamePlayerKey::Slash)
		{
			return gMouse->IsUp(keyboard[(UINT)key]);
		}
		else
		{
			return gKeyboard->IsUp(keyboard[(UINT)key]);
		}
	}

	bool Stroke(GamePlayerKey key)
	{
		if (key == GamePlayerKey::Slash)
		{
			return gMouse->IsDown(keyboard[(UINT)key]);
		}
		else
		{
			return gKeyboard->IsDown(keyboard[(UINT)key]);
		}
	}

	bool IsPressMovement(void)
	{
		bool res = false;
		res |= Pressed(GamePlayerKey::MoveForward);
		res |= Pressed(GamePlayerKey::MoveBackward);
		res |= Pressed(GamePlayerKey::MoveLeft);
		res |= Pressed(GamePlayerKey::MoveRight);
		return res;
	}

	bool IsPressRotate(void)
	{
		bool res = false;
		res |= Pressed(GamePlayerKey::RotateLeft);
		res |= Pressed(GamePlayerKey::RotateRight);
		res |= gMouse->IsPress(MOUSE_RBUTTON);

		return res;
	}
};