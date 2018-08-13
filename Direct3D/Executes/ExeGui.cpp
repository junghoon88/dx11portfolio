#include "stdafx.h"
#include "ExeGui.h"

#include "../Viewer/Free.h"

ExeGui::ExeGui(ExecuteValues* values)
	: Execute(values)
{
	LoadJsonFile();
}

ExeGui::~ExeGui()
{
	SaveJsonFile();
}

void ExeGui::Update(void)
{
	if (gKeyboard->IsPress(VK_CONTROL))
	{
		if (gKeyboard->IsDown(VK_F1))
			ChangeShowEnvironmentWindow();

		if (gKeyboard->IsDown(VK_F2))
			ChangeShowEditorSettingWindow();
		//if (gKeyboard->IsDown(VK_F2))
		//	ChangeShowLandscapeWindow();

		if (gKeyboard->IsDown(VK_F3))
			ChangeShowGlobalLightWindow();

		if (gKeyboard->IsDown(VK_F4))
			ChangeShowLightManagerWindow();

		if (gKeyboard->IsDown(VK_F11))
			ChangeShowSystemInfoWindow();

		if (gKeyboard->IsDown(VK_F12))
			ChangeShowDemoWindow();

	}
}

void ExeGui::Render(void)
{
}

void ExeGui::PostRender(void)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("EnvironmentWindow", "Ctrl+F1"))
				ChangeShowEnvironmentWindow();

			if (ImGui::MenuItem("EditorSetting", "Ctrl+F2"))
				ChangeShowEditorSettingWindow();
			if (ImGui::MenuItem("LandscapeWindow", ""))
				ChangeShowLandscapeWindow();

			if (ImGui::MenuItem("GlobalLightWindow", "Ctrl+F3"))
				ChangeShowGlobalLightWindow();

			if (ImGui::MenuItem("LightManagerWindow", "Ctrl+F4"))
				ChangeShowLightManagerWindow();


			if (ImGui::MenuItem("SystemInfo", "Ctrl+F11"))
				ChangeShowSystemInfoWindow();

			if (ImGui::MenuItem("Demo", "Ctrl+F12"))
				ChangeShowDemoWindow();


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	
	if (values->GuiSettings->bShowGlobalLightWindow)
	{
		ImGui::Begin("Global Light", &values->GuiSettings->bShowGlobalLightWindow);

		ImGui::SliderFloat3("Sun Direction", values->GlobalLight->Data.Direction, -1, 1);
		ImGui::ColorEdit3("Sun Color", values->GlobalLight->Data.Color);
		ImGui::SliderFloat("Overcast", &values->GlobalLight->Data.Overcast, 0.001f, 1.0f);

		ImGui::End();
	}


	D3DXVECTOR3 cameraPosition;
	D3DXVECTOR2 cameraRotation;
	values->MainCamera->GetPosition(&cameraPosition);
	values->MainCamera->GetRotationDegree(&cameraRotation);

	if (values->GuiSettings->bShowSystemInfoWindow)
	{
		ImGui::Begin("System Info", &values->GuiSettings->bShowSystemInfoWindow);
		ImGui::Text("Frame Per Second : %4.0f", ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::Text
		(
			"Camera Position : %3.0f, %3.0f, %3.0f"
			, cameraPosition.x, cameraPosition.y, cameraPosition.z
		);
		ImGui::Text
		(
			"Camera Rotation : %3.0f, %3.0f", cameraRotation.x, cameraRotation.y
		);

		bool isMovable = values->MainCamera->GetIsMovable();
		if (ImGui::Checkbox("Camera Movable", &isMovable))
		{
			values->MainCamera->SetIsMovable(isMovable);
		}

		Free* fpcamera = dynamic_cast<Free*>(values->MainCamera);
		if (fpcamera != NULL)
		{
			ImGui::SliderFloat("Camera Speed", fpcamera->GetMoveSpeedAddress(), 5, 1000);
		}


		ImGui::End();
	}

	if (values->GuiSettings->bShowDemoWindow)
	{
		ImGui::ShowDemoWindow(&values->GuiSettings->bShowDemoWindow);
	}


	if (values->GuiSettings->bShowEditorSettingWindow)
	{
		ImGui::Begin("Excutes Setting", &values->GuiSettings->bShowEditorSettingWindow);
		ImGui::Text("EditableFlag");
		{
			bool bEidtModel = values->editableFlag & (ULONG)ExcuteFlags::Model;
			if (ImGui::Checkbox("EditableModel", &bEidtModel))
			{
				values->editableFlag ^= (ULONG)ExcuteFlags::Model;
			}

			//bool bEidtTerrain = values->editableFlag & (ULONG)ExcuteFlags::Terrain;
			//if (ImGui::Checkbox("EditableTerrain", &bEidtTerrain))
			//{
			//	values->editableFlag ^= (ULONG)ExcuteFlags::Terrain;
			//}
			ImGui::Checkbox("EditableTerrain", &values->GuiSettings->bShowLandscapeWindow);
		}

		ImGui::Text("RenderFlag");
		{
			bool bRenderModel = (values->renderFlag & (ULONG)ExcuteFlags::Model) == (ULONG)ExcuteFlags::Model;
			if (ImGui::Checkbox("RenderModel", &bRenderModel))
			{
				values->renderFlag ^= (ULONG)ExcuteFlags::Model;
			}

			bool bRenderTerrain = (values->renderFlag & (ULONG)ExcuteFlags::Terrain) == (ULONG)ExcuteFlags::Terrain;
			if (ImGui::Checkbox("RenderTerrain", &bRenderTerrain))
			{
				values->renderFlag ^= (ULONG)ExcuteFlags::Terrain;
			}
		}
		ImGui::End();
	}

	if (values->GuiSettings->bShowLightManagerWindow)
	{
		ImGui::Begin("LightManager", &values->GuiSettings->bShowLightManagerWindow);
		gLight->PostRender();
		ImGui::End();
	}
}

void ExeGui::ChangeShowGlobalLightWindow(void)
{
	values->GuiSettings->bShowGlobalLightWindow ^= 1;
}

void ExeGui::ChangeShowEnvironmentWindow(void)
{
	values->GuiSettings->bShowEnvironmentWindow ^= 1;
}

void ExeGui::ChangeShowLandscapeWindow(void)
{
	values->GuiSettings->bShowLandscapeWindow ^= 1;
}

void ExeGui::ChangeShowSystemInfoWindow(void)
{
	values->GuiSettings->bShowSystemInfoWindow ^= 1;
}

void ExeGui::ChangeShowDemoWindow(void)
{
	values->GuiSettings->bShowDemoWindow ^= 1;
}

void ExeGui::ChangeShowEditorSettingWindow(void)
{
	values->GuiSettings->bShowEditorSettingWindow ^= 1;
}

void ExeGui::ChangeShowLightManagerWindow(void)
{
	values->GuiSettings->bShowLightManagerWindow ^= 1;
}

void ExeGui::LoadJsonFile(void)
{
	Json::Value globallight = (*values->jsonRoot)["Global Light"];
	if (globallight.isNull() == false)
	{
		Json::GetValue(globallight, "Direction", values->GlobalLight->Data.Direction);
		Json::GetValue(globallight, "Color", values->GlobalLight->Data.Color);
		Json::GetValue(globallight, "Overcast", values->GlobalLight->Data.Overcast);
	}
}

void ExeGui::SaveJsonFile(void)
{
	Json::Value globallight;
	Json::SetValue(globallight, "Direction", values->GlobalLight->Data.Direction);
	Json::SetValue(globallight, "Direction", values->GlobalLight->Data.Color);
	Json::SetValue(globallight, "Overcast", values->GlobalLight->Data.Overcast);

	(*values->jsonRoot)["Global Light"] = globallight;
}

