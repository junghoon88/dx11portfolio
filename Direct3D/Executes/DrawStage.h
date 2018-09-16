#pragma once

class GameModel;
class GamePlayer;
class GameEnemy;
class GamePlayManager;

namespace Landscape { class Terrain; class Ocean; }
namespace Environment { class Sky; }

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
	Landscape::Ocean* ocean;

	vector<GameModel*> staticModels;

	GamePlayManager*	gamePlayManager;
	//Player, Enemy
	GamePlayer*			player;
	vector<GameEnemy*>  enemies;
	vector<GameEnemy*>  bosses;
};