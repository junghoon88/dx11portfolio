#pragma once

class GameModel;
class BoundingSphere;
class BoundingBox;

#include "../Landscape/Terrain.h"
#include "../Environment/Sky.h"

class EditLevel : public Execute
{
public:
	EditLevel(ExecuteValues* values);
	~EditLevel();

	void Update(void);

	void PreRender(void);
	void Render(void);
	void PostRender(void);

	void ResizeScreen(void) {}

private:
	void LoadJsonFile(void);
	void SaveJsonFile(void);

	void EditPlayerStart(void);
	void LoadData(void);
	void SaveData(void);

private:
	wstring levelFile;
	Json::Value* levelJson;

	ExecuteValues* values;

	Landscape::Terrain* terrain;
	Environment::Sky* sky;

	vector<GameModel*> staticModels;
	
	BoundingSphere*			playerStart;
};