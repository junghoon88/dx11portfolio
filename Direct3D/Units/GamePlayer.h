#pragma once
#include "GameUnit.h"

struct GamePlayerSpec;
struct GamePlayerInput;

class GamePlayer : public GameUnit
{
public:
	enum class DebugMode;
private:
	enum class PlayerAction;

public:
	GamePlayer(wstring path, ANIMATION_TYPE animType = ANIMATION_TYPE::Mixamo);
	~GamePlayer();

	void Init(void);
	void Update(void);
	void Render(void);
	void PostRender(void);

private:
	void LinkAnimation(void);

	void HandleInput(void);
	void HandleMove(void);
	void HandleRotation(void);
	void HandleAttack(void);

	void SetPlayerIdle(void);
	void PlayAction(PlayerAction action);
	void PlayAction(PlayerAction action, float startTime);

	void ActionMovement(D3DXVECTOR3 direction, OUT D3DXVECTOR3& velocity);

public:
	inline DebugMode GetDebugMode(void) { return debugMode; }

private:
	GamePlayerSpec* specData;
	GamePlayerInput* input;

	D3DXVECTOR3 moveDirection;

private:
	DebugMode debugMode;

	PlayerAction curAction;
	PlayerAction nextAction;

	vector<UINT> indexAnimations;
	bool bRun;
	float attackTime;

public:
	enum class DebugMode
	{
		None = 0, NeverDie, Superman, God, 
	};
private:
	enum class PlayerAction
	{
		Idle, 
		Walk_Forward, Walk_Backward, Strafe_Left, Strafe_Right, Run,
		Slash, Slash_3combo, Kick, Charge_Attack, High_Spin_Attack,
		Power_Up,
		Spell_Cast,
		Death1, Death2,
		Count,
	};
};