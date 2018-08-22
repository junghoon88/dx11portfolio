#pragma once
#include "GameUnit.h"

struct GamePlayerSpec;
struct GamePlayerInput;
class BoundingBox;
class DrawModel;

class GamePlayer : public GameUnit
{
public:
	enum class DebugMode;
private:
	enum class LowerAction;
	enum class UpperAction;

public:
	GamePlayer(wstring path);
	~GamePlayer();

	void Update(void);
	void Render(void);
	void PostRender(void);

private:
	void LoadAnimation(wstring path);

	void HandleInput(void);
	void HandleMove(void);
	void HandleRotation(void);
	void HandleAttack(void);

	void PlayLowerAction(LowerAction action);
	void PlayLowerAction(LowerAction action, float startTime);
	void PlayUpperAction(UpperAction action);
	void PlayUpperAction(UpperAction action, float startTime);

	void ActionMovement(D3DXVECTOR3 direction);
	void ActionRotation(void);

public:
	void LinkAdress(DrawModel* p) { drawModel = p; }
	inline DebugMode GetDebugMode(void) { return debugMode; }

private:
	GamePlayerSpec* specData;
	GamePlayerInput* input;

	ModelBone* boneWaist;
	D3DXMATRIX boneWaistSrc;

	D3DXVECTOR3 moveDirection;

	float runDurationTime;
	float walkDurationTime;
	float boosterFinishDurationTim;
	float boosterBreakDurationTime;
	float weaponChangeDurationTime;

	bool bActiveBooster;
	bool bRun;

	float rootRotationAngle;
	float rootElapsedAngle;
	float curWaistAngle;

	bool bOverwriteLowerAction = false; // 동작이 바뀌는지 여부
	bool bOverwriteUpperAction = false; // 동작이 바뀌는지 여부
	float actionElapsedTime;
	float moveElapsedTime;

	//attack
	float attackDelay;
	float attackTime;
	int   attackNum;

	//box
	BoundingBox* box;

	//DrawModel
	DrawModel* drawModel;

private:
	DebugMode debugMode;

	LowerAction curLowerAction;
	UpperAction curUpperAction;

	LowerAction prepareLowerAction;
	UpperAction prepareUpperAction;

	vector<UINT> indexLowerAnimations;
	vector<UINT> indexUpperAnimations;

public:
	enum class DebugMode
	{
		None = 0, NeverDie, Superman, God, 
	};
private:
	enum class LowerAction
	{
		Unknown = 0,
		Idle, Run, Damage, Walk, BackWalk, LeftTurn, RightTurn,
		ForwardDead, BackwardDead, LeftDead, RightDead,
		BoosterPrepare, BoosterActive, BoosterFinish, BoosterBreak,
		BoosterLeftTurn, BoosterRightTurn,
		Count,
	};
	enum class UpperAction
	{
		Unknown = 0,
		Idle, Run, Damage, WeaponChange,
		ForwardNonFire, LeftNonFire, RightNonFire,
		ForwardMachineGunFire, LeftMachineGunFire, RightMachineGunFire, ReloadMachineGun,
		ForwardShotgunFire, LeftShotgunFire, RightShotgunFire, ReloadShotgun,
		ForwardHandgunFire, LeftHandgunFire, RightHandgunFire, ReloadHandgun,
		ForwardDead, BackwardDead, LeftDead, RightDead,
		BoosterPrepare, BoosterActive, BoosterFinish, BoosterBreak,
		BoosterLeftTurn, BoosterRightTurn,
		Count,
	};

};