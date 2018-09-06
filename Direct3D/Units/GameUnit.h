#pragma once
#include "../Objects/GameModel.h"

class GameUnit : public GameModel
{
public:
	GameUnit(wstring matmeshFile, ANIMATION_TYPE animType);
	virtual ~GameUnit();

	virtual void Update(void);
	virtual void Render(void);

	virtual void Attack(GameUnit* target);
	virtual void Hit(GameUnit* target);

public:
	virtual void ActionIdle(void) {}
	virtual void ActionHit(GameUnit* attacker) {}
	virtual void ActionDamage(GameUnit* attacker) {}
	virtual void ActionDead(D3DXVECTOR3 attackerPosition) {}


public:

protected:
	UINT  Level;
	float HP;
	float HPMax;
	float MP;
	float MPMax;
	float Exp;
	float ExpMax;
	float Atk;
	float Dep;


private:

};

