#include "stdafx.h"
#include "EditModel.h"

#include "../Objects/GameModel.h"
#include "../Model/Bounding/ModelShape.h"

#include "../Fbx/Exporter.h"

EditModel::EditModel(ExecuteValues* values)
	: Execute(values)
	, model(NULL)
{
	wstring fbxFile  = Assets + L"Meshes/Capsule.fbx";
	wstring savePath = Models + L"Meshes/";
	Fbx::Exporter::ExportMatMesh(fbxFile, savePath);

	modelShape = new ModelShape();
}

EditModel::~EditModel()
{
	SAFE_DELETE(modelShape);
}

void EditModel::Update(void)
{
	modelShape->ClearBuffer();

	modelShape->CreateBuffer();
}

void EditModel::PreRender(void)
{
}

void EditModel::Render(void)
{
	if (model)
	{
		model->Render();
	}

	modelShape->Render();
}

void EditModel::PostRender(void)
{
	PostRenderModelMenu();
	PostRenderModelProp();
}

void EditModel::PostRenderModelMenu(void)
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

void EditModel::PostRenderModelProp(void)
{
	if (model == NULL)
		return;

	model->GetModel();
}



void EditModel::OpenExportDialog(wstring file)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() == 0)
	{
		function<void(wstring)> f = bind(&EditModel::OpenExportDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::FbxModelFilter, Assets, f, desc.Handle);
	}
	else
	{
		wstring fileName = Path::GetFileNameWithoutExtension(file);
		function<void(wstring)> f = bind(&EditModel::Export, this, file, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::BinModelFilter, Models, f, desc.Handle);
	}
}

void EditModel::Export(wstring fbxFile, wstring saveFile)
{
	wstring savePath = Path::GetDirectoryName(saveFile);
	wstring saveName = Path::GetFileNameWithoutExtension(saveFile);

	Fbx::Exporter exporter(fbxFile);
	exporter.ExportMaterial(savePath, saveName);
	exporter.ExportMesh(savePath, saveName);
	exporter.ExportAnimation(savePath, saveName);
}

void EditModel::OpenModelDialog(wstring file)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() == 0)
	{
		function<void(wstring)> f = bind(&EditModel::OpenModelDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", L"", Models, f, desc.Handle);
	}
	else
	{
		wstring filePath = Path::GetDirectoryName(file);
		wstring fileName = Path::GetFileNameWithoutExtension(file);

		if (Path::ExistFile(filePath + fileName + L".material")
			&& Path::ExistFile(filePath + fileName + L".mesh"))
		{
			if (model)
			{
				SAFE_DELETE(model);
			}
			model = new GameModel(filePath);
		}
		else
		{
			MessageBox(desc.Handle, L"DrawModel::OpenModelDialog", L"Failed", MB_OK);
		}
	}
}
