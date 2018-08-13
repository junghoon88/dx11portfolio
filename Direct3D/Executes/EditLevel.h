#pragma once

class ModelShape;
class GameModel;

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
	void PostRenderModelMenu(void);
	void PostRenderModelProp(void);

	void OpenExportDialog(wstring file = L"");
	void Export(wstring fbxFile, wstring saveFile);
	void OpenModelDialog(wstring file = L"");

	void LoadJsonFile(void);
	void SaveJsonFile(void);


private:
	wstring levelFile;
	Json::Value* levelJson;

	ExecuteValues* values;
	ModelShape* modelShape;

	Landscape::Terrain* terrain;
	Environment::Sky* sky;

	vector<GameModel*> staticModels;



};