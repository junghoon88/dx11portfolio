#include "stdafx.h"
#include "DrawModel.h"

#include "../Fbx/Exporter.h"

#include "../Units/GameSettings.h"

DrawModel::DrawModel(ExecuteValues* values)
	: Execute(values)
{
	settings = new GameSettings(values);

}

DrawModel::~DrawModel()
{
	SAFE_DELETE(settings);
}

void DrawModel::Update(void)
{
	settings->Update();
}

void DrawModel::PreRender(void)
{
}

void DrawModel::Render(void)
{
	settings->Render();
}

void DrawModel::PostRender(void)
{
	//PostRenderModelMenu();

	settings->PostRender();
}

void DrawModel::PostRenderModelMenu(void)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Model"))
		{
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

void DrawModel::OpenExportDialog(wstring file)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() == 0)
	{
		function<void(wstring)> f = bind(&DrawModel::OpenExportDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::FbxModelFilter, Assets, f, desc.Handle);
	}
	else
	{
		wstring fileName = Path::GetFileNameWithoutExtension(file);
		function<void(wstring)> f = bind(&DrawModel::Export, this, file, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::BinModelFilter, Models, f, desc.Handle);
	}
}

void DrawModel::Export(wstring fbxFile, wstring saveFile)
{
	wstring savePath = Path::GetDirectoryName(saveFile);
	wstring saveName = Path::GetFileNameWithoutExtension(saveFile);

	Fbx::Exporter exporter(fbxFile);
	exporter.ExportMaterial(savePath, saveName);
	exporter.ExportMesh(savePath, saveName);
	exporter.ExportAnimation(savePath, saveName);
}

