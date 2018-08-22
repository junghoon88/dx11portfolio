#include "stdafx.h"
#include "EditLevel.h"

#include "../Fbx/Exporter.h"



EditLevel::EditLevel(ExecuteValues* values)
	: Execute(values)
	, levelFile(L""), levelJson(NULL)
	, values(values)
{
	terrain = new Landscape::Terrain(values);
	sky = new Environment::Sky(values);



	LoadJsonFile();
}

EditLevel::~EditLevel()
{
	SaveJsonFile();

	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);


	//for (GameModel* model : staticModels)
	//{
	//	SAFE_DELETE(model);
	//}
}

void EditLevel::Update(void)
{
	sky->Update();
	terrain->Update();


	//for (GameModel* model : staticModels)
	//{
	//	model->Update();
	//}

}

void EditLevel::PreRender(void)
{
}

void EditLevel::Render(void)
{
	sky->Render();
	terrain->Render();
}

void EditLevel::PostRender(void)
{
	sky->PostRender();
	terrain->PostRender();
}

void EditLevel::LoadJsonFile(void)
{
	Json::Value levelFileValue = (*values->jsonRoot)["LevelFile"];
	if (levelFileValue.isNull() == false)
	{
		Json::GetValue(levelFileValue, "FileName", levelFile);

		levelJson = new Json::Value();
		Json::ReadData(L"json/" + levelFile, levelJson);

		sky->LoadData(levelJson);
		terrain->LoadData(levelJson);
		gLight->LoadData(levelJson);
	}
}

void EditLevel::SaveJsonFile(void)
{
	if (levelFile.length() == 0 || levelJson == NULL)
		return;

	sky->SaveData(levelJson);
	terrain->SaveData(levelJson);
	gLight->SaveData(levelJson);

	Json::WriteData(L"json/" + levelFile, levelJson);
	SAFE_DELETE(levelJson);

	Json::Value levelFileValue;
	Json::SetValue(levelFileValue, "FileName", levelFile);
	(*values->jsonRoot)["LevelFile"] = levelFileValue;
}
