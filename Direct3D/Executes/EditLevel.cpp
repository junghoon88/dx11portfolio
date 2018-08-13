#include "stdafx.h"
#include "EditLevel.h"

#include "../Model/Bounding/ModelShape.h"

#include "../Fbx/Exporter.h"



EditLevel::EditLevel(ExecuteValues* values)
	: Execute(values)
	, levelFile(L""), levelJson(NULL)
	, values(values)
{
	modelShape = new ModelShape();

	terrain = new Landscape::Terrain(values);
	sky = new Environment::Sky(values);




	LoadJsonFile();
}

EditLevel::~EditLevel()
{
	SaveJsonFile();

	SAFE_DELETE(modelShape);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);

	//for (GameModel* model : staticModels)
	//{
	//	SAFE_DELETE(model);
	//}
}

void EditLevel::Update(void)
{
	modelShape->ClearBuffer();

	sky->Update();
	terrain->Update();



	//for (GameModel* model : staticModels)
	//{
	//	model->Update(modelShape);
	//}


	modelShape->CreateBuffer();
}

void EditLevel::PreRender(void)
{
}

void EditLevel::Render(void)
{
	sky->Render();
	terrain->Render();


	modelShape->Render();
}

void EditLevel::PostRender(void)
{
	PostRenderModelMenu();
	PostRenderModelProp();

	sky->PostRender();
	terrain->PostRender();
}

void EditLevel::PostRenderModelMenu(void)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Model"))
		{
			if (ImGui::MenuItem("Open", ""))
			{
				OpenModelDialog();
			}

			ImGui::Separator();
			if (ImGui::MenuItem("ExportFbx With Animation", ""))
			{
				OpenExportDialog();
			}


			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void EditLevel::PostRenderModelProp(void)
{
}



void EditLevel::OpenExportDialog(wstring file)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() == 0)
	{
		function<void(wstring)> f = bind(&EditLevel::OpenExportDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::FbxModelFilter, Assets, f, desc.Handle);
	}
	else
	{
		wstring fileName = Path::GetFileNameWithoutExtension(file);
		function<void(wstring)> f = bind(&EditLevel::Export, this, file, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::BinModelFilter, Models, f, desc.Handle);
	}
}

void EditLevel::Export(wstring fbxFile, wstring saveFile)
{
	wstring savePath = Path::GetDirectoryName(saveFile);
	wstring saveName = Path::GetFileNameWithoutExtension(saveFile);

	Fbx::Exporter exporter(fbxFile);
	exporter.ExportMaterial(savePath, saveName);
	exporter.ExportMesh(savePath, saveName);
	exporter.ExportAnimation(savePath, saveName);
}

void EditLevel::OpenModelDialog(wstring file)
{
	//D3DDesc desc;
	//D3D::GetDesc(&desc);

	//if (file.length() == 0)
	//{
	//	function<void(wstring)> f = bind(&EditLevel::OpenModelDialog, this, placeholders::_1);
	//	Path::OpenFileDialog(L"", L"", Models, f, desc.Handle);
	//}
	//else
	//{
	//	wstring filePath = Path::GetDirectoryName(file);
	//	wstring fileName = Path::GetFileNameWithoutExtension(file);

	//	if (Path::ExistFile(filePath + fileName + L".material")
	//		&& Path::ExistFile(filePath + fileName + L".mesh"))
	//	{
	//		if (model)
	//		{
	//			SAFE_DELETE(model);
	//		}
	//		model = new GameModel(filePath);
	//	}
	//	else
	//	{
	//		MessageBox(desc.Handle, L"DrawModel::OpenModelDialog", L"Failed", MB_OK);
	//	}
	//}
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
