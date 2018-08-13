#include "stdafx.h"
#include "Sky.h"
#include "SkyPlane.h"

#include "../Fbx/Exporter.h"


Environment::Sky::Sky(ExecuteValues* values)
	: shaderFile(Shaders + L"Sky.hlsl")
	, values(values)
{
	//Fbx::Exporter* exporter = NULL;
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Sphere.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/", L"Sphere");
	//exporter->ExportMesh(Models + L"Meshes/", L"Sphere");
	//SAFE_DELETE(exporter);

	wstring matFile  = Models + L"Meshes/Sphere.material";
	wstring meshFile = Models + L"Meshes/Sphere.mesh";

	sphere = new Model();
	sphere->ReadMaterial(matFile);
	sphere->ReadMesh(meshFile);

	for (Material* material : sphere->GetMaterials())
	{
		material->SetShader(shaderFile);
		material->SetDiffuseMap(Contents + L"Environment/Stars.png");
	}

	D3DXVECTOR3 min = D3DXVECTOR3(Math::FloatMaxValue, Math::FloatMaxValue, Math::FloatMaxValue);
	D3DXVECTOR3 max = D3DXVECTOR3(Math::FloatMinValue, Math::FloatMinValue, Math::FloatMinValue);

	for (ModelMesh* mesh : sphere->GetMeshesRef())
	{
		mesh->ScanPointMinMax(&min, &max);
	}


	skyBuffer = new SkyBuffer();

	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		States::CreateRasterizer(&desc, &rasterizer[0]);

		//Culling CCW로 변경 
		desc.FrontCounterClockwise = true;
		States::CreateRasterizer(&desc, &rasterizer[1]);
	}

	{
		D3D11_DEPTH_STENCIL_DESC desc;
		States::GetDepthStencilDesc(&desc);
		States::CreateDepthStencil(&desc, &depthStencilState[0]);

		desc.DepthEnable = false;
		States::CreateDepthStencil(&desc, &depthStencilState[1]);
	}

	wstring cloudFile   = Textures + L"cloud001.dds";
	wstring perturbFile = Textures + L"perturb001.dds";
	skyPlane = new SkyPlane(cloudFile, perturbFile);

	D3DXMatrixIdentity(&world);
}

Environment::Sky::~Sky()
{
	SAFE_DELETE(sphere);
	SAFE_DELETE(skyBuffer);

	SAFE_RELEASE(rasterizer[0]);
	SAFE_RELEASE(rasterizer[1]);

	SAFE_RELEASE(depthStencilState[0]);
	SAFE_RELEASE(depthStencilState[1]);
}

void Environment::Sky::LoadData(Json::Value* value)
{
	Json::Value sky = (*value)["Environment"]["Sky"];
	if (sky.isNull() == false)
	{
		Json::GetValue(sky, "Horizon Color Twilight",	skyBuffer->Data.HorizonColorTwilight);
		Json::GetValue(sky, "Horizon Color Day",		skyBuffer->Data.HorizonColorDay);
		Json::GetValue(sky, "Horizon Color Night",		skyBuffer->Data.HorizonColorNight);
		Json::GetValue(sky, "Ceiling Color Twilight",	skyBuffer->Data.CeilingColorTwilight);
		Json::GetValue(sky, "Ceiling Color Day",		skyBuffer->Data.CeilingColorDay);
		Json::GetValue(sky, "Ceiling Color Night",		skyBuffer->Data.CeilingColorNight);
		Json::GetValue(sky, "Horizon Height",			skyBuffer->Data.Height);
	}

	skyPlane->LoadData(value);
}

void Environment::Sky::SaveData(Json::Value* value)
{
	Json::Value sky;

	Json::SetValue(sky, "Horizon Color Twilight",	skyBuffer->Data.HorizonColorTwilight);
	Json::SetValue(sky, "Horizon Color Day",		skyBuffer->Data.HorizonColorDay);
	Json::SetValue(sky, "Horizon Color Night",		skyBuffer->Data.HorizonColorNight);
	Json::SetValue(sky, "Ceiling Color Twilight",	skyBuffer->Data.CeilingColorTwilight);
	Json::SetValue(sky, "Ceiling Color Day",		skyBuffer->Data.CeilingColorDay);
	Json::SetValue(sky, "Ceiling Color Night",		skyBuffer->Data.CeilingColorNight);
	Json::SetValue(sky, "Horizon Height",			skyBuffer->Data.Height);

	(*value)["Environment"]["Sky"] = sky;

	skyPlane->SaveData(value);
}

void Environment::Sky::Update(void)
{
	//항상 카메라를 따라다니도록
	D3DXVECTOR3 position;
	values->MainCamera->GetPosition(&position);

	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, 2, 2, 2);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z + 1);
	world = S * T;

	skyBuffer->Data.Time = gTime->RunningTime() * 10;

	skyPlane->Update();
}

void Environment::Sky::Render(void)
{
	//깊이버퍼 끄기
	gDC->OMSetDepthStencilState(depthStencilState[1], 1);
	//Culling CCW로 변경 
	gDC->RSSetState(rasterizer[1]);

	skyBuffer->SetPSBuffer(2);
	for (ModelMesh* mesh : sphere->GetMeshesRef())
	{
		mesh->SetWorld(world);
		mesh->Render();
	}

	//Culling CW로 원상복귀 
	gDC->RSSetState(rasterizer[0]);
	
	//구름 랜더링
	skyPlane->Render();
	
	//깊이버퍼 다시 켜기
	gDC->OMSetDepthStencilState(depthStencilState[0], 1);

}

void Environment::Sky::PostRender(void)
{
	if (values->GuiSettings->bShowEnvironmentWindow)
	{
		ImGui::Begin("Environment Window", &values->GuiSettings->bShowEnvironmentWindow);

		ImGui::Text("Sky Settings");

		ImGui::ColorEdit3("Horizon Color Twilight",	skyBuffer->Data.HorizonColorTwilight);
		ImGui::ColorEdit3("Horizon Color Day",		skyBuffer->Data.HorizonColorDay);
		ImGui::ColorEdit3("Horizon Color Night",	skyBuffer->Data.HorizonColorNight);
		ImGui::ColorEdit3("Ceiling Color Twilight",	skyBuffer->Data.CeilingColorTwilight);
		ImGui::ColorEdit3("Ceiling Color Day",		skyBuffer->Data.CeilingColorDay);
		ImGui::ColorEdit3("Ceiling Color Night",	skyBuffer->Data.CeilingColorNight);
		ImGui::SliderFloat("Horizon Height",		&skyBuffer->Data.Height, 0.03f, 1.0f);

		ImGui::Separator();
		skyPlane->PostRender();

		ImGui::End();
	}
}

