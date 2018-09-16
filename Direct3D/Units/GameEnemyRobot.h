#pragma once
#include "GameEnemy.h"

struct GameEnemyRobotSpec;

class GameEnemyRobot : public GameEnemy
{
private:
	enum class EnemyRobotAction;

public:
	GameEnemyRobot(wstring matmeshFile);
	~GameEnemyRobot();

	void Init(void);
	virtual void Update(void) override;
	virtual void Render(void) override;

	virtual void ActionHit(GameUnit* victim) override;		//공격 했을 때
	virtual void ActionDamage(GameUnit* attacker) override;	//공격 당했을 때

private:
	void LinkAnimation(void);

	void ActionIdle(void);
	void ActionFollow(D3DXVECTOR3 dir, float dist);
	void ActionAttack(D3DXVECTOR3 dir, float dist);
	void ActionDamage(void);
	void ActionDeath(void);


	void PlayAction(EnemyRobotAction action, float startTime = 0.0f);
	void LookatPlayer(D3DXVECTOR3 dir);

private:
	virtual void OnAiIdle(AiState* state) override;
	virtual void OnAiFollow(AiState* state) override;
	virtual void OnAiAttack(AiState* state) override;
	virtual void OnAiDamage(AiState* state) override;
	virtual void OnAiDeath(AiState* state) override;

private:
	GameEnemyRobotSpec* specData;
	vector<UINT> indexAnimations;

	EnemyRobotAction curAction;

	float attackDelay;

private:
	enum class EnemyRobotAction
	{
		Idle, Run, 
		Paunch, Kick, Damage, Death,
		Count,
	};
};
