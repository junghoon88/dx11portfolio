#include "stdafx.h"
#include "MeshCube.h"

MeshCube::MeshCube()
	: GameModel(Models + L"Meshes/Cube")
{
	diffuse = D3DXCOLOR(0, 0, 0, 1);
	specular = D3DXCOLOR(1, 1, 1, 1);
	shininess = 15.0f;

	shader = new Shader(Shaders + L"999_NormalMap.hlsl");

	for (Material* material : model->GetMaterials())
	{
		material->SetShader(shader);
		material->SetDiffuseMap( Textures + L"Wall.png");
		material->SetSpecularMap(Textures + L"Wall_specular.png");
		material->SetNormalMap(Textures + L"Wall_normal.png");
	}

	buffer = new Buffer();
}

MeshCube::~MeshCube()
{
	SAFE_DELETE(buffer);
}

void MeshCube::Update()
{
	GameModel::Update();

	buffer->SetVSBuffer(4);
}

void MeshCube::Render()
{
	GameModel::Render();
	
	ImGui::SliderFloat3("Diffuse",  diffuse, 0, 1);
	ImGui::SliderFloat3("Specular", specular, 0, 1);
	ImGui::SliderFloat("Shininess", &shininess, 0.1f, 20);

	ImGui::SliderFloat3("Clip Direction", (float*)&buffer->Data.ClipPlane, -1, 1);
	ImGui::SliderFloat("Clip Position", &buffer->Data.ClipPlane.w, -10, 10);
	ImGui::Separator();

	for (Material* material : model->GetMaterials())
	{
		material->SetDiffuse(diffuse);
		material->SetSpecular(specular);
		material->SetShininess(shininess);
	}
}

void MeshCube::PostRender(void)
{
}
