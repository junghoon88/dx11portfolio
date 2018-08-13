#include "stdafx.h"
#include "LightManager.h"
#include "PointLight.h"
#include "SpotLight.h"

LightManager* LightManager::instance = NULL;

void LightManager::Update(void)
{
	pointLight->Update(pointLights);
	spotLight->Update(spotLights);
}

void LightManager::PostRender(void)
{
	ImGui::Text("Point Lights");
	{
		for (size_t i = 0; i < pointLights.size(); i++)
		{
			if (ImGui::Button(String::ToString(String::WSInt(L"%d.", i) + pointLights[i].first).c_str()))
			{
				showPointLightIndex = i;
				showSpotLightIndex = -1;
			}
			ImGui::SameLine();
			if (ImGui::Button(String::SInt("DeletePL%d", i).c_str()))
			{
				showPointLightIndex = -1;
				DeletePointLight(pointLights[i].first);
			}

			if (showPointLightIndex == i)
			{
				ImGui::DragFloat3("Position",	pointLights[showPointLightIndex].second.Position, 0.2f);
				ImGui::ColorEdit3("Color",		pointLights[showPointLightIndex].second.Color);
				ImGui::DragFloat("Intensity",  &pointLights[showPointLightIndex].second.Intensity, 0.1f, 0.1f, 10.0f);
				ImGui::DragFloat("Range",	   &pointLights[showPointLightIndex].second.Range, 0.1f, 0.0f, 20.0f);
			}
		}


		if (inputText1) 
		{
			ImGui::InputText("Name", nameBuf1, 128);

			if (ImGui::Button("OK"))
			{
				PointLightData data;
				AddPointLight(String::ToWString(nameBuf1), data);
				inputText1 = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("CANCLE"))
			{
				inputText1 = false;
			}
		}

		if (ImGui::Button("Add Point Light"))
		{
			inputText1 = true;
			sprintf_s(nameBuf1, 128, "PL-%02d", pointLights.size());
		}
	}

	ImGui::Separator();
	ImGui::Text("Spot Lights");
	{
		for (size_t i = 0; i < spotLights.size(); i++)
		{
			if (ImGui::Button(String::ToString(String::WSInt(L"%d.", i) + spotLights[i].first).c_str()))
			{
				showPointLightIndex = -1;
				showSpotLightIndex = i;
			}
			ImGui::SameLine();
			if (ImGui::Button(String::SInt("DeleteSL%d", i).c_str()))
			{
				showSpotLightIndex = -1;
				DeleteSpotLight(spotLights[i].first);
			}

			if (showSpotLightIndex == i)
			{
				ImGui::DragFloat3("Position",	spotLights[showSpotLightIndex].second.Position, 0.2f);
				ImGui::ColorEdit3("Color",		spotLights[showSpotLightIndex].second.Color);
				ImGui::DragFloat3("Direction",	spotLights[showSpotLightIndex].second.Direction, 0.1f, -1.0f, 1.0f);
				ImGui::DragFloat("InnerAngle", &spotLights[showSpotLightIndex].second.InnerAngle, 0.01f, spotLights[showSpotLightIndex].second.OuterAngle, 1.0f);
				ImGui::DragFloat("OuterAngle", &spotLights[showSpotLightIndex].second.OuterAngle, 0.01f, 0.0f, spotLights[showSpotLightIndex].second.InnerAngle);
				ImGui::DragFloat("Range",	   &spotLights[showSpotLightIndex].second.Range, 0.1f, 0.0f, 20.0f);
			}
		}


		if (inputText2)
		{
			ImGui::InputText("Name", nameBuf2, 128);

			if (ImGui::Button("OK"))
			{
				SpotLightData data;
				AddSpotLight(String::ToWString(nameBuf2), data);
				inputText2 = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("CANCLE"))
			{
				inputText2 = false;
			}
		}

		if (ImGui::Button("Add Spot Light"))
		{
			inputText2 = true;
			sprintf_s(nameBuf2, 128, "SL-%02d", spotLights.size());
		}
	}
}

void LightManager::AddPointLight(wstring name, PointLightData data)
{
	for (size_t i = 0; i < pointLights.size(); i++)
	{
		if (pointLights[i].first == name)
		{
			//assert(false);
			pointLights[i].second = data;
		}
	}

	pointLights.push_back(make_pair(name, data));
}

void LightManager::DeletePointLight(wstring name)
{
	for (size_t i = 0; i < pointLights.size(); i++)
	{
		if (pointLights[i].first == name)
		{
			pointLights.erase(pointLights.begin() + i);
			return;
		}
	}
}

PointLightData* LightManager::GetPointLight(wstring name)
{
	for (size_t i = 0; i < pointLights.size(); i++)
	{
		if (pointLights[i].first == name)
		{
			return &pointLights[i].second;
		}
	}

	return nullptr;
}

void LightManager::AddSpotLight(wstring name, SpotLightData data)
{
	for (size_t i = 0; i < spotLights.size(); i++)
	{
		if (spotLights[i].first == name)
		{
			//assert(false);
			spotLights[i].second = data;
		}
	}

	spotLights.push_back(make_pair(name, data));
}

void LightManager::DeleteSpotLight(wstring name)
{
	for (size_t i = 0; i < spotLights.size(); i++)
	{
		if (spotLights[i].first == name)
		{
			spotLights.erase(spotLights.begin() + i);
			return;
		}
	}
}

SpotLightData* LightManager::GetSpotLight(wstring name)
{
	for (size_t i = 0; i < spotLights.size(); i++)
	{
		if (spotLights[i].first == name)
		{
			return &spotLights[i].second;
		}
	}

	return nullptr;
}

LightManager::LightManager()
{
	pointLight = new PointLight();
	spotLight = new SpotLight();
}

LightManager::~LightManager()
{
	SAFE_DELETE(pointLight);
	SAFE_DELETE(spotLight);
}

void LightManager::LoadData(Json::Value * value)
{
	//Point Light
	{
		Json::Value PLValue = (*value)["Lights"]["Point Light"];
		if (PLValue.isNull() == false)
		{
			UINT lightCount = 0;

			Json::GetValue(PLValue, "Light Count", lightCount);
			wstring name = L"";
			PointLightData data;
			for (UINT i = 0; i < lightCount; i++)
			{
				Json::GetValue(PLValue, String::SInt("Name%d",      i), name);
				Json::GetValue(PLValue, String::SInt("Position%d",  i), data.Position);
				Json::GetValue(PLValue, String::SInt("Color%d",     i), data.Color);
				Json::GetValue(PLValue, String::SInt("Intensity%d", i), data.Intensity);
				Json::GetValue(PLValue, String::SInt("Range%d",     i), data.Range);

				AddPointLight(name, data);
			}
		}
	}

	//Spot Light
	{
		Json::Value SLValue = (*value)["Lights"]["Spot Light"];
		if (SLValue.isNull() == false)
		{
			UINT lightCount = 0;

			Json::GetValue(SLValue, "Light Count", lightCount);

			wstring name = L"";
			SpotLightData data;
			for (UINT i = 0; i < lightCount; i++)
			{
				Json::GetValue(SLValue, String::SInt("Name%d",       i), name);
				Json::GetValue(SLValue, String::SInt("Position%d",   i), data.Position);
				Json::GetValue(SLValue, String::SInt("Color%d",      i), data.Color);
				Json::GetValue(SLValue, String::SInt("Direction%d",  i), data.Direction);
				Json::GetValue(SLValue, String::SInt("InnerAngle%d", i), data.InnerAngle);
				Json::GetValue(SLValue, String::SInt("OuterAngle%d", i), data.OuterAngle);
				Json::GetValue(SLValue, String::SInt("Range%d",      i), data.Range);
				
				AddSpotLight(name, data);
			}
		}
	}
}

void LightManager::SaveData(Json::Value * value)
{
	//Point Light
	{
		Json::Value PLValue;

		UINT lightCount = pointLights.size();
		Json::SetValue(PLValue, "Light Count", lightCount);
		for (UINT i = 0; i < lightCount; i++)
		{
			Json::SetValue(PLValue, String::SInt("Name%d",      i), pointLights[i].first);
			Json::SetValue(PLValue, String::SInt("Position%d",  i), pointLights[i].second.Position);
			Json::SetValue(PLValue, String::SInt("Color%d",     i), pointLights[i].second.Color);
			Json::SetValue(PLValue, String::SInt("Intensity%d", i), pointLights[i].second.Intensity);
			Json::SetValue(PLValue, String::SInt("Range%d",     i), pointLights[i].second.Range);
		}

		(*value)["Lights"]["Point Light"] = PLValue;
	}

	//Spot Light
	{
		Json::Value SLValue;

		UINT lightCount = spotLights.size();
		Json::SetValue(SLValue, "Light Count", lightCount);
		for (UINT i = 0; i < lightCount; i++)
		{
			Json::SetValue(SLValue, String::SInt("Name%d",       i), spotLights[i].first);
			Json::SetValue(SLValue, String::SInt("Position%d",   i), spotLights[i].second.Position);
			Json::SetValue(SLValue, String::SInt("Color%d",      i), spotLights[i].second.Color);
			Json::SetValue(SLValue, String::SInt("Direction%d",  i), spotLights[i].second.Direction);
			Json::SetValue(SLValue, String::SInt("InnerAngle%d", i), spotLights[i].second.InnerAngle);
			Json::SetValue(SLValue, String::SInt("OuterAngle%d", i), spotLights[i].second.OuterAngle);
			Json::SetValue(SLValue, String::SInt("Range%d",      i), spotLights[i].second.Range);
		}

		(*value)["Lights"]["Spot Light"] = SLValue;
	}

}
