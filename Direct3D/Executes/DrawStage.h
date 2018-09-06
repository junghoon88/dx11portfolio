#pragma once

class GameModel;
class GamePlayer;
class GameEnemy;

#include "../Landscape/Terrain.h"
#include "../Environment/Sky.h"

class DrawStage : public Execute
{
public:
	DrawStage(ExecuteValues* values);
	~DrawStage();

	void Update(void);

	void PreRender(void);
	void Render(void);
	void PostRender(void);

	void ResizeScreen(void) {}

private:
	void LoadJsonFile(void);
	void SaveJsonFile(void);

	void LoadLevelData(Json::Value* levelJson);

public:
	inline Landscape::Terrain* GetTerrain(void) { return terrain; }
	inline GamePlayer* GetPlayer(void) { return player; }
	inline void AddEnemy(GameEnemy* enemy) { enemies.push_back(enemy); }


private:
	wstring levelFile;
	Json::Value* levelJson;

	ExecuteValues* values;

	Landscape::Terrain* terrain;
	Environment::Sky* sky;

	vector<GameModel*> staticModels;


	//Player, Enemy
	GamePlayer*			player;
	vector<GameEnemy*>  enemies;
	vector<GameEnemy*>  bosses;
};