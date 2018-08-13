#include "stdafx.h"
#include "MeshCapsule.h"

MeshCapsule::MeshCapsule()
	: GameModel(Models + L"Meshes/Capsule")
{
	diffuse = D3DXCOLOR(0, 0, 0, 1);
	specular = D3DXCOLOR(1, 1, 1, 1);
	shininess = 15.0f;


	shader = new Shader(Shaders + L"999_Specular.hlsl");

	for (Material* material : model->GetMaterials())
	{
		material->SetShader(shader);
	}
}

MeshCapsule::~MeshCapsule()
{
}

void MeshCapsule::Update(ModelShape * modelShape)
{
	GameModel::Update(modelShape);

	for (Material* material : model->GetMaterials())
	{
		material->SetDiffuse(diffuse);
		material->SetSpecular(specular);
		material->SetShininess(shininess);
	}
}

void MeshCapsule::Render()
{
	GameModel::Render();
	
	ImGui::SliderFloat3("Diffuse",  diffuse, 0, 1);
	ImGui::SliderFloat3("Specular", specular, 0, 1);
	ImGui::SliderFloat("Shininess", &shininess, 0.1f, 20);
}

void MeshCapsule::PostRender(void)
{
}
