#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Free.h"
#include "./Viewer/FixedCamera.h"
#include "./Viewer/FrustumCulling.h"


#include "./Executes/EditModel.h"
#include "./Executes/EditLevel.h"
#include "./Executes/DrawStage.h"
#include "./Executes/ExeGui.h"

Program::Program()
{
	States::Create();

	InitializeValues();
	InitializeExecutes();
}


Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	Json::WriteData(L"json/LevelEditor.json", values->jsonRoot);

	SAFE_DELETE(values->jsonRoot);
	SAFE_DELETE(values->FrustumCulling);
	SAFE_DELETE(values->MainCamera);
	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->GlobalTime);
	SAFE_DELETE(values->GuiSettings);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values);

	States::Delete();
}

void Program::Update(void)
{
	UpdateCommand();

	values->MainCamera->Update();


	D3DXVECTOR3 up = D3DXVECTOR3(0, 1, 0);
	float dot = D3DXVec3Dot(&values->GlobalLight->Data.Direction, &up);
	float intensity = Math::Clamp(dot * 1.5f, 0.0f, 1.0f);
	values->GlobalLight->Data.Intensity = intensity;

	values->GlobalTime->Data.ElapsedTime = gTime->Delta();
	values->GlobalTime->Data.RunningTime = gTime->RunningTime();

	gLight->Update();

	gModelShape->ClearBuffer();

	for (Execute* exe : executes)
		exe->Update();

	gModelShape->CreateBuffer();
}

void Program::PreRender(void)
{
}

void Program::Render(void)
{
	D3D::Get()->SetRenderTarget(NULL);

	SetGlobalBuffers();

	for (Execute* exe : executes)
		exe->Render();

	gModelShape->Render();
}

void Program::PostRender(void)
{
	for (Execute* exe : executes)
		exe->PostRender();

#if 0
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Excute Menu"))
		{
			if (ImGui::MenuItem("Edit Level", ""))
			{
				SAFE_DELETE(executes[0]);
				executes[0] = new EditLevel(values);
			}
			if (ImGui::MenuItem("Edit Model", ""))
			{
				SAFE_DELETE(executes[0]);
				executes[0] = new EditModel(values);
	
			}
			if (ImGui::MenuItem("Draw Stage", ""))
			{
				SAFE_DELETE(executes[0]);
				executes[0] = new DrawStage(values);
			}
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
#endif
}

void Program::ResizeScreen(void)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values->Perspective->Set(desc.Width, desc.Height);
	values->Viewport->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}

void Program::InitializeValues(void)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);
	values->GuiSettings = new GuiSettings();
	values->GlobalLight = new LightBuffer();
	values->GlobalTime = new TimeBuffer();

	//values->MainCamera = new Free(50);
	//values->MainCamera->SetPosition(122, 209, -118);
	//values->MainCamera->SetRotationDegree(48, 0);

	values->MainCamera = new FixedCamera();
	values->MainCamera->SetPosition(0, 10, 10);
	values->MainCamera->SetRotationDegree(10, 0);

	values->FrustumCulling = new FrustumCulling(500.0f, values->MainCamera, values->Perspective);

	values->jsonRoot = new Json::Value();
	Json::ReadData(L"json/LevelEditor.json", values->jsonRoot);


	values->editableFlag = 0;
	values->renderFlag = (ULONG)ExcuteFlags::All;
}

void Program::InitializeExecutes(void)
{
	executes.push_back(new EditLevel(values));
	//executes.push_back(new EditModel(values));
	//executes.push_back(new DrawStage(values));
	executes.push_back(new ExeGui(values));
}

void Program::UpdateCommand(void)
{
	//카메라 지진효과
	//if (gKeyboard->IsDown('Q'))
	//{
	//	values->MainCamera->Shake(10.0f, 2.0f);
	//}

	//뒤로가기
	if (gKeyboard->IsDown(VK_CONTROL, 'Z'))
	{
		gCmdManager->UndoCommand();
	}

	//되돌리기
	if (gKeyboard->IsDown(VK_CONTROL, 'Y'))
	{
		gCmdManager->RedoCommand();
	}
}

void Program::SetGlobalBuffers(void)
{
	D3DXMATRIX view, projection;
	values->MainCamera->GetMatrix(&view);
	values->Perspective->GetMatrix(&projection);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	values->GlobalLight->SetPSBuffer(0);
	values->GlobalTime->SetVSBuffer(10);
}

void Program::LoadJsonFile(void)
{
}

void Program::SaveJsonFile(void)
{
}
