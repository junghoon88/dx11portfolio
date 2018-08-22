#pragma once

#include "GamePlayer.h"
#include "../Landscape/Terrain.h"

class EnemyTank;

class PlayerBullet;

namespace Landscape { class Terrain; }

class GameSettings
{
public:
	GameSettings(ExecuteValues* values);
	~GameSettings();

	void Update(void);
	void Render(void);
	void PostRender(void);

private:
	void CreateEnvironment(void);
	void CreatePlayer(void);
	void CreateEnemy(void);

public:
	inline static GamePlayer* GetPlayer(void) { return player; }
	inline static Landscape::Terrain* GetTerrain(void) { return terrain; }


private:
	ExecuteValues* values;

	static GamePlayer* player;
	static Landscape::Terrain* terrain;
	EnemyTank* tank;

	vector<GameModel*> staticModels;
	vector<PlayerBullet*> bullets;
};
