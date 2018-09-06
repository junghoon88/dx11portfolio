#include "stdafx.h"
#include "DrawStage.h"

#include "../Units/GamePlayer.h"
#include "../Units/GameEnemy.h"
#include "../Objects/GModelReadWrite.h"
#include "../Viewer/FollowCamera.h"

#include "../Model/Bounding/BoundingBox.h"

DrawStage::DrawStage(ExecuteValues* values)
	: Execute(values)
	, levelFile(L""), levelJson(NULL)
	, values(values)
{
	terrain = new Landscape::Terrain(values);
	sky = new Environment::Sky(values);

	wstring file = Models + L"Maria/Maria2.gmodel";
	GModelReadWriter::OpenGModel(file, GModelReadWriter::GModelReadClass::GamePlayer, (void**)&player);
	player->Init();

	SAFE_DELETE(values->MainCamera);
	values->MainCamera = new FollowCamera(player, 80.0f);

	LoadJsonFile();
}

DrawStage::~DrawStage()
{
	SaveJsonFile();

	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);

	SAFE_DELETE(player);

	//for (GameModel* model : staticModels)
	//{
	//	SAFE_DELETE(model);
	//}
}

void DrawStage::Update(void)
{
	sky->Update();
	terrain->Update();

	player->Update();
	//for (GameModel* model : staticModels)
	//{
	//	model->Update();
	//}

}

void DrawStage::PreRender(void)
{
}

void DrawStage::Render(void)
{
	sky->Render();
	terrain->Render();

	player->Render();
}

void DrawStage::PostRender(void)
{
	sky->PostRender();
	terrain->PostRender();

	player->PostRender();
}

void DrawStage::LoadJsonFile(void)
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
		this->LoadLevelData(levelJson);
	}

	vector<EnemyArea> areas = terrain->GetEnemyAreasRef();
	
}

void DrawStage::SaveJsonFile(void)
{
}

void DrawStage::LoadLevelData(Json::Value* levelJson)
{
	Json::Value value = (*levelJson)["Positions"];

	if (value.isNull() == false)
	{
		D3DXVECTOR3 position;
		D3DXCOLOR color;
		Json::GetValue(value, string("Player Position"), position);
		player->SetPosition(position);
	}
}
