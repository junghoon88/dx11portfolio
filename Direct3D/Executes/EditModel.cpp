#include "stdafx.h"
#include "EditModel.h"

#include "../Objects/GameModel.h"

#include "../Fbx/Exporter.h"
#include "../Landscape/Grid3Axis.h"

#include "../Model/Bounding/BoundingCapsule.h"

EditModel::EditModel(ExecuteValues* values)
	: Execute(values)
	, fileName(L""), model(NULL)
{
	//wstring fbxFile  = Assets + L"Paladin/Paladin.fbx";
	//wstring savePath = Models + L"Paladin/";
	//Fbx::Exporter::ExportMatMesh(fbxFile, savePath);

	grid = new Grid3Axis();
}

EditModel::~EditModel()
{
	SAFE_DELETE(model);
	SAFE_DELETE(grid);
}

void EditModel::Update(void)
{
	if (model)
	{
		D3DXMATRIX root = transform.GetMatrix();
		model->SetRootAxis(root);
		model->Update();

		size_t capsuleCount = model->capsules.size();
		for (size_t i = 0; i < capsuleCount; i++)
		{
			gModelShape->AddBoundingCapsule(model->capsules[i], model->capsules[i]->GetColorRef());
		}
	}
}

void EditModel::PreRender(void)
{
}

void EditModel::Render(void)
{
	grid->Render();
	if (model)
	{
		model->Render();

	}
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
			if (ImGui::MenuItem("Open MatMesh Model", ""))
			{
				OpenMatMeshDialog();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Open GameModel File", ""))
			{
				OpenGameModelDialog();
			}
			if (ImGui::MenuItem("Save GameModel File", ""))
			{
				SaveGameModelDialog();
			}



			ImGui::Separator();

			if (ImGui::MenuItem("ExportFbx MatMesh", ""))
			{
				OpenExportDialog(EXPORT_MODE::EXPORT_MODE_MATMESH);
			}
			if (ImGui::MenuItem("ExportFbx Animations", ""))
			{
				OpenExportDialog(EXPORT_MODE::EXPORT_MODE_ANIMATIONS);
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

	ShowTreeNode();
	ShowTransform();
	ShowAnimationInfo();
	ShowCapsuleInfo();
}



void EditModel::OpenExportDialog(EXPORT_MODE mode, wstring file)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() == 0)
	{
		function<void(wstring)> f = bind(&EditModel::OpenExportDialog, this, mode, placeholders::_1);
		Path::OpenFileDialog(L"", /*Path::FbxModelFilter*/L"", Assets, f, desc.Handle);
	}
	else
	{
		switch (mode)
		{
			case EXPORT_MODE::EXPORT_MODE_MATMESH:
			{
				wstring fileName = Path::GetFileNameWithoutExtension(file);
				function<void(wstring)> f = bind(&EditModel::ExportMatMesh, this, file, placeholders::_1);
				Path::SaveFileDialog(fileName, Path::BinModelFilter, Models, f, desc.Handle);
			}
			break;
			case EXPORT_MODE::EXPORT_MODE_ANIMATIONS:
			{
				ExportAnimations(Path::GetDirectoryName(file));
			}
			break;
		}
	}
}

void EditModel::ExportMatMesh(wstring fbxFile, wstring saveFile)
{
	wstring savePath = Path::GetDirectoryName(saveFile);
	wstring saveName = Path::GetFileNameWithoutExtension(saveFile);

	Fbx::Exporter exporter(fbxFile);
	exporter.ExportMaterial(savePath, saveName);
	exporter.ExportMesh(savePath, saveName);
}

void EditModel::ExportAnimations(wstring fbxPath)
{
	vector<wstring> fbxFiles;
	Path::GetFiles(&fbxFiles, fbxPath, L"*.fbx", true);

	wstring savePath = fbxPath;
	String::Replace(&savePath, Assets, Models);

	for (size_t i = 0; i < fbxFiles.size(); i++)
	{
		Fbx::Exporter exporter(fbxFiles[i]);

		wstring fileName = Path::GetFileNameWithoutExtension(fbxFiles[i]);
		exporter.ExportAnimation(savePath, fileName);
	}


}

void EditModel::OpenMatMeshDialog(wstring file)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() == 0)
	{
		function<void(wstring)> f = bind(&EditModel::OpenMatMeshDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", L"", Models, f, desc.Handle);
	}
	else
	{
		wstring filePath = Path::GetDirectoryName(file);
		wstring fileName = Path::GetFileNameWithoutExtension(file);

		if (Path::ExistFile(filePath + fileName + L".material")
			&& Path::ExistFile(filePath + fileName + L".mesh"))
		{
			SAFE_DELETE(model);
			this->fileName = filePath + fileName;
			model = new GameModel(filePath + fileName, ANIMATION_TYPE::Mixamo);
			transform = TRANSFORM();
		}
		else
		{
			MessageBox(desc.Handle, L"DrawModel::OpenMatMeshDialog", L"Failed", MB_OK);
		}
	}
}

void EditModel::OpenAnimationDialog(wstring file)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() == 0)
	{
		function<void(wstring)> f = bind(&EditModel::OpenAnimationDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::AnimFilter, Models, f, desc.Handle);
	}
	else
	{
		model->AddClip(file);
	}
}

void EditModel::OpenGameModelDialog(wstring file)
{
	SAFE_DELETE(model);

	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() == 0)
	{
		function<void(wstring)> f = bind(&EditModel::OpenGameModelDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::GameModelFilter, Models, f, desc.Handle);
	}
	else
	{
		Json::Value root;
		Json::ReadData(file, &root);
		Json::Value val = root["Model Property"];
		wstring matFile, meshFile;
		D3DXMATRIX matRoot;
		int animType = -1;
		int clipCount = 0;
		vector<pair<wstring, bool>> clipNames;

		//1. model - matFile, meshFile
		{
			Json::GetValue(val, "matFile",  matFile);
			Json::GetValue(val, "meshFile", meshFile);
		}
		//2. model - rootAxis
		{
			Json::GetValue(val, "RootTransformS", transform.Scale);
			Json::GetValue(val, "RootTransformR", transform.RotationDeg);
			Json::GetValue(val, "RootTransformT", transform.Position);
		}
		//3. animation - type
		{
			Json::GetValue(val, "AnimType", animType);
		}
		//4. animation - clips
		{
			Json::GetValue(val, "AnimClipCount", clipCount);

			wstring name;
			bool bLoop = false;
			for (int i = 0; i < clipCount; i++)
			{
				Json::GetValue(val, String::SInt("AnimClip%d", i), name);
				Json::GetValue(val, String::SInt("AnimClipLoop%d", i), bLoop);
				clipNames.push_back(make_pair(name, bLoop));
			}
		}

		wstring file = Path::GetDirectoryName(matFile) + Path::GetFileNameWithoutExtension(matFile);
		model = new GameModel(file, (ANIMATION_TYPE)animType);
		model->SetRootAxis(matRoot);
		for (int i = 0; i < clipCount; i++)
		{
			model->AddClip(clipNames[i].first, clipNames[i].second);
		}
	}
}

void EditModel::SaveGameModelDialog(wstring file)
{
	if (model == NULL)
		return;

	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() == 0)
	{
		function<void(wstring)> f = bind(&EditModel::SaveGameModelDialog, this, placeholders::_1);
		Path::SaveFileDialog(L"", Path::GameModelFilter, Models, f, desc.Handle);
	}
	else
	{
		Json::Value root;
		Json::Value val;
		//1. model - matFile, meshFile
		{
			wstring matFile, meshFile;
			model->GetFileInfo(matFile, meshFile);
			Json::SetValue(val, "matFile",  matFile);
			Json::SetValue(val, "meshFile", meshFile);
		}
		//2. model - rootAxis
		{
			Json::SetValue(val, "RootTransformS", transform.Scale);
			Json::SetValue(val, "RootTransformR", transform.RotationDeg);
			Json::SetValue(val, "RootTransformT", transform.Position);
		}
		//3. animation - type
		{
			int animType = (int)model->GetAnimType();
			Json::SetValue(val, "AnimType", animType);
		}
		//4. animation - clips
		{
			int clipCount = model->GetClipsRef().size();
			Json::SetValue(val, "AnimClipCount", clipCount);

			for (int i = 0; i < clipCount; i++)
			{
				Json::SetValue(val, String::SInt("AnimClip%d", i), model->GetClip(i)->fileName);
				Json::SetValue(val, String::SInt("AnimClipLoop%d", i), model->GetClip(i)->bLoop);
			}
		}

		root["Model Property"] = val;
		Json::WriteData(file, &root);
	}
}

void EditModel::ShowTreeNode(void)
{
	ImGui::Begin("Model Bone Tree");
	{
		model->GetModel()->ShowTreeNode();
	}
	ImGui::End();
}

void EditModel::ShowTransform(void)
{
	ImGui::Begin("Model RootAxis");
	{
		ImGui::DragFloat3("Scale", transform.Scale, 0.1f);
		ImGui::DragFloat3("Rotation", transform.RotationDeg, 10.0f);
		ImGui::DragFloat3("Position", transform.Position, 1.0f);
	}
	ImGui::End();
}

void EditModel::ShowAnimationInfo(void)
{
	ImGui::Begin("EditModel Animation Clip");
	{
		UINT count = model->GetClipsRef().size();
		bool deleteAnim = false;
		UINT removeIndex = 0;

		for (UINT i = 0; i < count; i++)
		{
			string name = String::ToString(model->GetClip(i)->fileName);
			if (ImGui::Button(name.c_str()))
			{
				model->PlayAnim(i);
			}
			ImGui::SameLine();
			ImGui::Checkbox(String::SInt("Loop%d", i).c_str(), &(model->GetClip(i)->bLoop));
			ImGui::SameLine();
			if (ImGui::Button(String::SInt("Delete%d", i).c_str()))
			{
				deleteAnim = true;
				removeIndex = i;
			}
		}

		if (deleteAnim)
		{
			model->RemoveClip(removeIndex);
		}


		ImGui::Separator();
		if (ImGui::Button("Play"))
		{
			model->PauseAnim(false);
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause"))
		{
			model->PauseAnim(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			model->StopAnim();
		}

		ImGui::ProgressBar(model->GetPlayProgress());

		if (ImGui::Button("Add Clip"))
		{
			OpenAnimationDialog();
		}
	}
	ImGui::End();
}

void EditModel::ShowCapsuleInfo(void)
{
	if (ImGui::Begin("Capsule List"))
	{
		size_t capsuleCount = model->capsules.size();
		static int showIndex = -1;
		int removeIndex = -1;
		for (size_t i = 0; i < capsuleCount; i++)
		{
			if (ImGui::Button(String::SInt("show info %d", i).c_str()))
			{
				showIndex = i;
			}
			ImGui::SameLine();
			if (ImGui::Button(String::SInt("delete info %d", i).c_str()))
			{
				removeIndex = i;
				if (showIndex == i)
					showIndex = -1;
			}
		}

		if (showIndex != -1)
		{
			ImGui::Separator();
			model->capsules[showIndex]->PostRender();
		}

		if (removeIndex != -1)
		{
			model->capsules.erase(model->capsules.begin() + removeIndex);
		}

	
		if (ImGui::Button("add"))
		{
			BoundingCapsule* capsule = new BoundingCapsule((GameUnit*)model, D3DXVECTOR3(0, 0, 0), 5, 10);
			model->capsules.push_back(capsule);
		}
	}
	ImGui::End();
}
