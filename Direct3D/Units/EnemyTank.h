#pragma once
#include "GameEnemy.h"

class EnemyTank : public GameEnemy
{
private:
	enum class Action;

public:
	EnemyTank(wstring path);
	~EnemyTank();

	void Update(ModelShape* modelShape);
	void Render(void);
	void PostRender(void);

	void ActionIdle(void);
	bool ActionFire(void);
	void ActionHit(GameUnit* attacker);
	bool ActionReload(GameWeapon* weapon);
	void ActionDamage(GameUnit* attacker);
	void ActionDead(D3DXVECTOR3 attackerPosition);


private:
	void ProcessAction(void);

	void OnAiSearch(AiState* state);
	void OnAiMove(AiState* state);
	void OnAiAttack(AiState* state);
	void OnAiTurnLeft(AiState* state);
	void OnAiTurnRight(AiState* state);


public:
	inline void PlayAction(Action action) { curAction = action; }

private:
	Action curAction;
	Action engageAction;

	UINT indexFireWeaponBone;
	UINT indexTurretBone;

	D3DXMATRIX transformTurret;
	float turretAngleSpeed;

private:
	enum class Action
	{
		Unknown = 0,
		Idle, Damage, Move, Fire, Reload, LeftTrun, RightTurn, Dead,
		
		Count
	};
};
