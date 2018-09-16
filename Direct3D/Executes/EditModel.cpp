#include "stdafx.h"
#include "EditModel.h"

#include "../Objects/GameModel.h"
#include "../Objects/GModelReadWrite.h"

#include "../Fbx/Exporter.h"
#include "../Landscape/Grid3Axis.h"

#include "../Model/Bounding/BoundingBox.h"
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

		if (gMouse->IsDown(MOUSE_LBUTTON))
		{
			D3DXVECTOR3 start, direction;
			float dist;
			values->MainCamera->GetPosition(&start);
			direction = values->MainCamera->GetDirection(values->Viewport, values->Perspective);

			if (model->MousePickked(start, direction, dist))
			{
				printf("");
			}
		}
	}
}

void EditModel::PreRender(void)
{
}

void EditModel::Render(void)
{
	grid->Render();

	if (model
		&& (values->renderFlag & (ULONG)ExcuteFlags::Model))
	{
		model->Render();
	}
}

void EditModel::PostRender(void)
{
	PostRenderModelMenu();
	PostRenderModelProp();
	PostRenderCamera();
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

	ShowModelInfo();
	ShowTreeNode();
	ShowTransform();
	ShowAnimationInfo();
	ShowBoundingInfo();
}

void EditModel::PostRenderCamera(void)
{
	float offset = 50.0f;

	ImGui::Begin("Camera View");
	{
		if (ImGui::Button("Top View"))
		{
			values->MainCamera->SetPosition(0, offset, 0);
			values->MainCamera->SetRotationDegree(90, 0);
		}
		if (ImGui::Button("Bottom View"))
		{
			values->MainCamera->SetPosition(0, -offset, 0);
			values->MainCamera->SetRotationDegree(-90, 0);
		}
		if (ImGui::Button("Left View"))
		{
			values->MainCamera->SetPosition(offset, 10, 0);
			values->MainCamera->SetRotationDegree(0, -90);
		}
		if (ImGui::Button("Right View"))
		{
			values->MainCamera->SetPosition(-offset, 10, 0);
			values->MainCamera->SetRotationDegree(0, 90);
		}
		if (ImGui::Button("Forward View"))
		{
			values->MainCamera->SetPosition(0, 10, -offset);
			values->MainCamera->SetRotationDegree(0, 0);
		}
		if (ImGui::Button("Backward View"))
		{
			values->MainCamera->SetPosition(0, 10, offset);
			values->MainCamera->SetRotationDegree(0, 180);
		}
	}
	ImGui::End();
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
		//
		GModelReadWriter::OpenGModel(file, GModelReadWriter::GModelReadClass::GameModel, (void**)&model);
		D3DXMATRIX matRoot;
		model->GetRootAxis(matRoot);
		MathDX::DecomposeTransform(matRoot, transform.Scale, transform.RotationDeg, transform.Position);
		transform.RotationDeg = Math::ToDegree(transform.RotationDeg);
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
			Json::SetValue(val, "shaderFile", model->GetShader()->GetFile());
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
		//5. Boundings
		{
			UINT count = model->boundings.size();
			Json::SetValue(val, "BoundingCount", count);


			for (UINT i = 0; i < count; i++)
			{
				Json::Value valBounding;

				Json::SetValue(valBounding, String::SInt("Type", i), (int)(model->boundings[i]->GetBoundingType()));
				Json::SetValue(valBounding, String::SInt("Prop", i), (int)(model->boundings[i]->GetBoundingProp()));
				Json::SetValue(valBounding, String::SInt("Name", i), model->boundings[i]->GetName());
				Json::SetValue(valBounding, String::SInt("SocketNum", i), model->boundings[i]->GetSocketnum());
				Json::SetValue(valBounding, String::SInt("Color", i), model->boundings[i]->GetColorRef());

				TRANSFORM tr = model->boundings[i]->GetTransformRef();
				Json::SetValue(valBounding, String::SInt("TR-RotationDeg", i), tr.RotationDeg);
				Json::SetValue(valBounding, String::SInt("TR-Position", i), tr.Position);


				switch (model->boundings[i]->GetBoundingType())
				{
					case BoundingType::Box:
					{
						Json::SetValue(valBounding, String::SInt("Box-Min", i), dynamic_cast<BoundingBox*>(model->boundings[i])->GetOrgMin());
						Json::SetValue(valBounding, String::SInt("Box-Max", i), dynamic_cast<BoundingBox*>(model->boundings[i])->GetOrgMax());
					}
					break;
					case BoundingType::Sphere:
					{

					}
					break;
					case BoundingType::Capsule:
					{
						Json::SetValue(valBounding, String::SInt("Capsule-Center", i), dynamic_cast<BoundingCapsule*>(model->boundings[i])->GetCenterRef());
						Json::SetValue(valBounding, String::SInt("Capsule-Radius", i), dynamic_cast<BoundingCapsule*>(model->boundings[i])->GetRadiusRef());
						Json::SetValue(valBounding, String::SInt("Capsule-Height", i), dynamic_cast<BoundingCapsule*>(model->boundings[i])->GetHeightRef());
					}
					break;
				}
				val[String::SInt("Bounding%d", i).c_str()] = valBounding;
			}
		}

		root["Model Property"] = val;
		Json::WriteData(file, &root);
	}
}

void EditModel::ShowModelInfo(void)
{
	ImGui::Begin("Model Info");
	{
		ImGui::Text("Shader File");
		string shaderName = String::ToString(model->GetShader()->GetFile());
		if (ImGui::Button(shaderName.c_str()))
		{
			D3DDesc desc;
			D3D::GetDesc(&desc);

			function<void(wstring)> f = bind(&GameModel::SetShader, model, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ShaderFilter, Shaders, f, desc.Handle);
		}
	}
	ImGui::End();
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

void EditModel::ShowBoundingInfo(void)
{
	if (ImGui::Begin("Bounding List"))
	{
		size_t count = model->boundings.size();
		static int showIndex = -1;
		int removeIndex = -1;
		for (size_t i = 0; i < count; i++)
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
			model->boundings[showIndex]->PostRender();
		}

		if (removeIndex != -1)
		{
			model->boundings.erase(model->boundings.begin() + removeIndex);
		}

	
		if (ImGui::Button("add Box"))
		{
			BoundingBox* box = new BoundingBox(model, D3DXVECTOR3(-1, -1, -1), D3DXVECTOR3(1, 1, 1));
			model->boundings.push_back(box);
		}
		ImGui::SameLine();
		if (ImGui::Button("add Capsule"))
		{
			BoundingCapsule* capsule = new BoundingCapsule(model, D3DXVECTOR3(0, 0, 0), 5, 10);
			model->boundings.push_back(capsule);
		}
	}
	ImGui::End();
}
