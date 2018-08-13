#include "stdafx.h"
#include "GameWeapon.h"
#include "GameData.h"

GameWeapon::GameWeapon()
{
	specData = NULL;
	state = WeaponState::Ready;
}

GameWeapon::~GameWeapon()
{
	SAFE_DELETE(specData);
}
