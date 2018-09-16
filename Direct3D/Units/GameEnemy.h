#pragma once
#include "GameUnit.h"

class AiContext;
struct AiState;
class GamePlayer;

class GameEnemy : public GameUnit
{
public:
	enum class AiType;

public:
	GameEnemy(wstring matmeshFile, ANIMATION_TYPE animType = ANIMATION_TYPE::Mixamo);
	virtual ~GameEnemy();

	virtual void Update(void);
	virtual void Render(void);

	virtual void ActionIdle(void) {}
	virtual void ActionHit(GameUnit* victim) {}			//공격 했을 때
	virtual void ActionDamage(GameUnit* attacker) {}	//공격 당했을 때
	virtual void ActionDead(D3DXVECTOR3 attackerPosition) {}


protected:
	void SetStartAi(AiType type);
	float CalcDistancePlayer(void);
	void CalcDistancePlayer(D3DXVECTOR3& dir, float& dist);

private:
	void OnIdle(AiState* state);
	void OnFollow(AiState* state);
	void OnAttack(AiState* state);
	void OnDamage(AiState* state);
	void OnDeath(AiState* state);

protected:
	virtual void OnAiIdle(AiState* state) = 0;
	virtual void OnAiFollow(AiState* state) = 0;
	virtual void OnAiAttack(AiState* state) = 0;
	virtual void OnAiDamage(AiState* state) = 0;
	virtual void OnAiDeath(AiState* state) = 0;

public:
	inline void LinkAddress(GamePlayer* player) { this->player = player; }


protected:	
	typedef pair<UINT, AiState*> AiPair;

protected:
	AiContext* aiContext;

	float actionElapsedTime;

	AiType startAi;

	AiPair aiIdle;
	AiPair aiFollow;
	AiPair aiAttack;
	AiPair aiDamage;
	AiPair aiDeath;

	GamePlayer* player;

	UnitHpBar* hpBar;

public:
	enum class AiType
	{
		Idle = 0, Follow, Attack, Damage, Death, 
	};
};