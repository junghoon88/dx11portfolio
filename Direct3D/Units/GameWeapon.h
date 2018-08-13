#pragma once
#include "../Objects/GameRender.h"

struct GameWeaponSpec;
enum class WeaponType;

class GameWeapon : public GameRender
{
public:
	enum class WeaponState;

public:
	GameWeapon();
	~GameWeapon();

	virtual void Update(ModelShape* modelShape) override {}
	virtual void Render(void) override {}
	virtual void PostRender(void) override {}

public:
	inline GameWeaponSpec* GetSpecData(void) { return specData; }
	inline void SetSpecData(GameWeaponSpec* val) { specData = val; }
	//inline WeaponType GetWeaponType(void) { return specData->Type; }
	inline WeaponState GetState(void) { return state; }

private:
	vector<Model*> models;

	GameWeaponSpec* specData;
	WeaponState state;

	vector<UINT> indexWeaponFireDummy; //ÃÑ±¸
	vector<UINT> indexWeaponAttackDummy; //

public:
	enum class WeaponState
	{
		Ready = 0, Firing, Reloading,

		Count
	};
};
