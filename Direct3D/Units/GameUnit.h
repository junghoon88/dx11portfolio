#pragma once
#include "../Objects/GameAnimationModel.h"

class GameWeapon;

class GameUnit : public GameAnimationModel
{
public:
	GameUnit(wstring path);
	virtual ~GameUnit();

	virtual void Update(ModelShape* modelShape);
	virtual void Render(void);

	virtual void Attack(GameUnit* target);
	virtual void Hit(GameUnit* target);

	bool MousePickked(BoundingFrustum* cameraFrustum);
	bool MousePickked(D3DXVECTOR3 start, D3DXVECTOR3 dir, OUT map<float, ModelMeshPart*>& pickkedParts);

	void CreateWeapon(wstring matFile, wstring meshFile);
	void SelectWeapon(UINT slot);

public:
	virtual void ActionIdle(void) {}
	virtual bool ActionFire(void) { return false; }
	virtual bool ActionMelee(void) { return false; }
	virtual void ActionHit(GameUnit* attacker) {}
	virtual bool ActionReload(GameWeapon* weapon) { return false; }
	virtual void ActionDamage(GameUnit* attacker) {}
	virtual void ActionDead(D3DXVECTOR3 attackerPosition) {}


public:
	inline void SetLife(float val) { life = val; }
	inline float GetLife(void) { return life; }
	inline void SetMaxLife(float val) { maxLife = val; }
	inline float GetMaxLife(void) { return maxLife; }

	inline void SetSpawnPoint(D3DXMATRIX& val) { spawnPoint = val; }
	inline D3DXMATRIX SetSpawnPoint(void) { return spawnPoint; }

	inline bool IsFullLife(void) { return (life == maxLife); }
	inline bool IsDead(void) { return (life < 1.0f); }

	inline GameWeapon* GetDefaultWeapon(void) { assert(weapons.size() > 0);  return weapons[0]; }
	inline GameWeapon* GetCurWeapon(void) { return curWeapon; }
	inline GameWeapon* GetWeapon(UINT index) { assert(index < weapons.size());  return weapons[index]; }

protected:
	D3DXVECTOR3 min;
	D3DXVECTOR3 max;

	D3DXMATRIX spawnPoint;

	GameWeapon* curWeapon;
	int curWeaponSlot;
	vector<GameWeapon*> weapons;

	float life;
	float maxLife;

private:

};

