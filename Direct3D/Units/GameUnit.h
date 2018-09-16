#pragma once
#include "../Objects/GameModel.h"

struct UnitInfo
{
	UINT  Level;
	float HP;
	float HPMax;
	float MP;
	float MPMax;
	float Exp;
	float ExpMax;
	float Atk;
	float Dep;
};

class UnitHpBar;

class GameUnit : public GameModel
{
public:
	GameUnit(wstring matmeshFile, ANIMATION_TYPE animType);
	virtual ~GameUnit();

	virtual void Update(void);
	virtual void Render(void);

public:
	virtual void ActionHit(GameUnit* victim);		//공격 했을 때
	virtual void ActionDamage(GameUnit* attacker);	//공격 당했을 때


public:
	inline bool GetValid(void) { return valid; }
	inline bool GetIsLive(void) { return isLive; }


protected:
	bool  valid;
	bool  isLive;

	UnitInfo unitInfo;
private:

};

