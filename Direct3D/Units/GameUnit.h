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
	virtual void ActionHit(GameUnit* victim);		//���� ���� ��
	virtual void ActionDamage(GameUnit* attacker);	//���� ������ ��


public:
	inline bool GetValid(void) { return valid; }
	inline bool GetIsLive(void) { return isLive; }


protected:
	bool  valid;
	bool  isLive;

	UnitInfo unitInfo;
private:

};

