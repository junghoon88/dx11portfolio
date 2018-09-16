#include "stdafx.h"
#include "DrawStage.h"

#include "../Units/GamePlayer.h"
#include "../Units/GameEnemy.h"
#include "../Objects/GModelReadWrite.h"
#include "../Viewer/FollowCamera.h"

#include "../Landscape/Terrain.h"
#include "../Environment/Sky.h"
#include "../Landscape/Ocean.h"
#include "../Landscape/TerrainData.h"

#include "../Model/Bounding/BoundingBox.h"

#include "../GamePlayManager/GamePlayManager.h"


DrawStage::DrawStage(ExecuteValues* values)
	: Execute(values)
	, levelFile(L""), levelJson(NULL)
	, values(values)
{
	terrain = new Landscape::Terrain(values);
	sky = new Environment::Sky(values);
	ocean = new Landscape::Ocean(values, 64, 64, 4, 0, 0);

	wstring file = Models + L"Maria/Maria2.gmodel";
	GModelReadWriter::OpenGModel(file, GModelReadWriter::GModelReadClass::GamePlayer, (void**)&player);
	player->Init();

	SAFE_DELETE(values->MainCamera);
	values->MainCamera = new FollowCamera(player, 80.0f);

	gamePlayManager = new GamePlayManager(this);

	LoadJsonFile();

	ocean->SetHeightMap(terrain->GetTerrainData()->GetHeightMap());

}

DrawStage::~DrawStage()
{
	SaveJsonFile();

	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
	SAFE_DELETE(ocean);

	SAFE_DELETE(player);

	SAFE_DELETE(gamePlayManager);

	//for (GameModel* model : staticModels)
	//{
	//	SAFE_DELETE(model);
	//}
}

void DrawStage::Update(void)
{
	sky->Update();
	terrain->Update();
	ocean->Update();

	gamePlayManager->Update();


	player->Update();

	for (size_t i = 0; i < enemies.size();)
	{
		if (enemies[i]->GetValid() == false)
		{
			enemies.erase(enemies.begin() + i);
		}
		else
		{
			enemies[i]->Update();
			++i;
		}
	}

	gBoundingCollision->CollisionCheck();

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
	ocean->Render();

	player->Render();

	for (GameEnemy* enemy : enemies)
	{
		enemy->Render();
	}
}

void DrawStage::PostRender(void)
{
	//sky->PostRender();
	//terrain->PostRender();

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
		ocean->LoadData(levelJson);
		gLight->LoadData(levelJson);
		this->LoadLevelData(levelJson);
	}

	vector<EnemyArea> areas = terrain->GetEnemyAreasRef();
	size_t count = areas.size();
	for (size_t i = 0; i < count; i++)
	{
		gamePlayManager->GetEnemyRespawnManager()->AddArea(areas[i]);
	}
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
