#pragma once
#include "GameUnit.h"

struct GameEnemySpec;
enum class UnitClassId;
class AiContext;
struct AiState;

class BoundingBox;
class DrawModel;


class GameEnemy : public GameUnit
{
public:
	enum class AiType;

public:
	GameEnemy(wstring path);
	virtual ~GameEnemy();

	virtual void Update(ModelShape* modelShape);
	virtual void Render(void);

	virtual void ActionIdle(void) {}
	virtual bool ActionFire(void) { return false; }
	virtual bool ActionMelee(void) { return false; }
	virtual void ActionHit(GameUnit* attacker) {}
	virtual bool ActionReload(GameWeapon* weapon) { return false; }
	virtual void ActionDamage(GameUnit* attacker) {}
	virtual void ActionDead(D3DXVECTOR3 attackerPosition) {}

	void SetStartAi(AiType type, float time);
	void NextAi(AiType type, float time);

private:
	void OnSearch(AiState* state);
	void OnMove(AiState* state);
	void OnAttack(AiState* state);
	void OnTurnLeft(AiState* state);
	void OnTurnRight(AiState* state);

protected:
	virtual void OnAiSearch(AiState* state) {};
	virtual void OnAiMove(AiState* state) {};
	virtual void OnAiAttack(AiState* state) {};
	virtual void OnAiTurnLeft(AiState* state) {};
	virtual void OnAiTurnRight(AiState* state) {};


protected:
	typedef pair<UINT, AiState*> AiPair;
protected:
	UnitClassId unitClass;
	GameEnemySpec* specData;
	AiContext* aiContext;

	float actionElapsedTime;

	AiType startAi;
	float startAiTime;

	AiPair aiSearch;
	AiPair aiMove;
	AiPair aiAttack;
	AiPair aiTurnLeft;
	AiPair aiTurnRight;

	//box
	BoundingBox* box;

	//DrawModel
	DrawModel* drawModel;


public:
	enum class AiType
	{
		Search = 0, Move, Attack, TurnLeft, TurnRight,
	};
};