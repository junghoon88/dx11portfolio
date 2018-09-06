#include "stdafx.h"
#include "EditLevel.h"

#include "../Fbx/Exporter.h"

#include "../Model/Bounding/BoundingSphere.h"


EditLevel::EditLevel(ExecuteValues* values)
	: Execute(values)
	, levelFile(L""), levelJson(NULL)
	, values(values)
{
	terrain = new Landscape::Terrain(values);
	sky = new Environment::Sky(values);

	playerStart = new BoundingSphere(NULL);
	playerStart->SetShow(true);

	LoadJsonFile();
}

EditLevel::~EditLevel()
{
	SaveJsonFile();

	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);

	SAFE_DELETE(playerStart);


	//for (GameModel* model : staticModels)
	//{
	//	SAFE_DELETE(model);
	//}
}

void EditLevel::Update(void)
{
	sky->Update();
	terrain->Update();

	playerStart->Update();

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

	EditPlayerStart();
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
		this->LoadData();
	}
}

void EditLevel::SaveJsonFile(void)
{
	if (levelFile.length() == 0 || levelJson == NULL)
		return;

	sky->SaveData(levelJson);
	terrain->SaveData(levelJson);
	gLight->SaveData(levelJson);
	this->SaveData();
	

	Json::WriteData(L"json/" + levelFile, levelJson);
	SAFE_DELETE(levelJson);

	Json::Value levelFileValue;
	Json::SetValue(levelFileValue, "FileName", levelFile);
	(*values->jsonRoot)["LevelFile"] = levelFileValue;
}

void EditLevel::EditPlayerStart(void)
{
	if (ImGui::Begin("Player Start"))
	{
		ImGui::DragFloat3("Position", playerStart->Center, 0.1f, -1000.0f, 1000.0f);
		playerStart->Center.y = terrain->GetVertexHeight(playerStart->Center);
		ImGui::ColorEdit3("Color", playerStart->GetColorRef());
		ImGui::Checkbox("Show", &playerStart->GetShowRef());
	}
	ImGui::End();
}


void EditLevel::LoadData(void)
{
	Json::Value value = (*levelJson)["Positions"];

	if (value.isNull() == false)
	{
		D3DXVECTOR3 position;
		D3DXCOLOR color;
		Json::GetValue(value, string("Player Position"), position);
		Json::GetValue(value, string("Player Color"), color);
		playerStart->Center = position;
		playerStart->SetColor(color);
	}
}

void EditLevel::SaveData(void)
{
	Json::Value value;
	Json::SetValue(value, string("Player Position"), playerStart->Center);
	Json::SetValue(value, string("Player Color"), playerStart->GetColorRef());

	(*levelJson)["Positions"] = value;
}
