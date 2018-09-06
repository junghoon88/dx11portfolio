#pragma once
#include "GameEnemy.h"

struct GameEnemyRobotSpec;

class GameEnemyRobot : public GameEnemy
{
public:
	GameEnemyRobot(wstring matmeshFile);
	~GameEnemyRobot();

	virtual void Update(void) override;
	virtual void Render(void) override;

	//virtual void ActionIdle(void) override;
	//virtual void ActionHit(GameUnit* attacker) override;
	//virtual void ActionDamage(GameUnit* attacker) override;
	//virtual void ActionDead(D3DXVECTOR3 attackerPosition) override;

private:
	virtual void OnAiIdle(AiState* state) override;
	virtual void OnAiFollow(AiState* state) override;
	virtual void OnAiAttack(AiState* state) override;

private:
	GameEnemyRobotSpec* specData;

};
