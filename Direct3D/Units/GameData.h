#pragma once
#include "stdafx.h"

enum class UnitClassId
{
	Unknown = 0,
	LightMech, HeavyMech, Tank, Boss,

	Count
};

enum class UnitTypeId
{
	Unknown = 0,
	//Player
	Grund, Kiev, Mark, Yager,

	//Enemy
	Cameleer, Maoming, Duskmas, Hammer, Tiger, PhantomBoss,

	Count
};

enum class WeaponType
{
	Unknown = 0,
	CameleerGun, MaomingGun, 
	DuskmasCannon, HammerCannon, TigerCannon, 
	PhantomMelee,
	
	PlayerMachineGun, PlayerHangun, PlayerShotgun,

	Count,
};

struct GameMeshSpec
{
	wstring DefaultWeaponFile = L"";
	wstring AnimationFolder = L"";

};

struct GameDataSpec : public GameMeshSpec
{
	wstring file;

	UnitTypeId Type = UnitTypeId::Grund;
	UnitClassId Class = UnitClassId::LightMech;

	UINT Life = 3500;
	float MechRadius = 2.0f;

	float TurnAngle = 45.0f;

	wstring MaterialFile = L"";
	wstring MeshFile = L"";
};

struct GamePlayerSpec : public GameDataSpec
{
	float RunSpeed = 7.0f;
	float WalkSpeed = 5.0f;
	float WalkBackwardSpeed = 4.0f;
	float BoosterSpeed = 32.0f;
	float BoosterActiveTime = 1.5f;
	float BoosterPrepareTime = 0.5f;
	float BoosterCoolTime = 8.0f;
	float BoosterTurnAngle = 70.0f;
	float CriticalDamage = 1.0f;


	D3DXVECTOR3 CameraTargetOffset = D3DXVECTOR3(0, 2, 0);
	D3DXVECTOR3 CameraPositionOffset = D3DXVECTOR3(0, 0, -5);
};

struct GameEnemySpec : public GameDataSpec
{
	float MoveSpeed = 4.0f;
};

struct GameWeaponSpec : public GameMeshSpec
{
	WeaponType Type = WeaponType::TigerCannon;

	int Damage = 300;

	UINT FireCount = 1;
	float FireRange = 70;
	float FireIntervalTime = 3.0f;
	float FireDelayTimeTillFirst = 0.0f;

	int FireHorizontalTilltAngle = 2;
	int FireVerticalTilltAngle = 1;

	bool CriticalDamagedFire = true;

	int TotalBulletCount = -1;
	UINT ReloadBulletCount = 5;
	float ReloadIntervalTime = 4.0f;


	float ModelRadius = 0.0f;

	bool ModelAlone = false;
	UINT ModelCount = 0;

	bool TracerBulletFire = true;
	float TracerBulletSpeed = 200.0f;
	float TracerBulletLength = 3.0f;
	float TracerBulletThickness = 0.7f;
};