#include "stdafx.h"
#include "MeshQuad.h"

MeshQuad::MeshQuad()
	: GameModel(Models + L"Meshes/Cube")
{
	shader = new Shader(Shaders + L"999_NormalMap.hlsl");

}

MeshQuad::~MeshQuad()
{
	SAFE_DELETE(shader);
}

void MeshQuad::Update()
{
	GameModel::Update();
}

void MeshQuad::Render()
{
	GameModel::Render();
}

void MeshQuad::PostRender(void)
{
}

void MeshQuad::SetShader(wstring file)
{
	SAFE_DELETE(shader);
	shader = new Shader(file);
}

void MeshQuad::SetDiffuse(D3DXCOLOR color)
{
	vector<Material*> materials = model->GetMaterials();
	for (Material* mat : materials)
		mat->SetDiffuse(color);
}

void MeshQuad::SetDiffuse(wstring file)
{
	vector<Material*> materials = model->GetMaterials();
	for (Material* mat : materials)
		mat->SetDiffuseMap(file);
}

void MeshQuad::SetSpecular(D3DXCOLOR color)
{
	vector<Material*> materials = model->GetMaterials();
	for (Material* mat : materials)
		mat->SetSpecular(color);
}

void MeshQuad::SetSpecular(wstring file)
{
	vector<Material*> materials = model->GetMaterials();
	for (Material* mat : materials)
		mat->SetSpecularMap(file);
}
