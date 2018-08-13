#include "stdafx.h"
#include "GameTerrain.h"
#include "TerrainData.h"

Landscape::GameTerrain::GameTerrain(ExecuteValues* values)
	: values(values)
{
	material = new Material(Shaders + L"GameTerrain.hlsl");
	material->SetDiffuseMap(Contents + L"Landscape/Dirt3.png");
	material->SetDetailMap(Contents + L"Landscape/Dirt3_detail.png");

	worldBuffer = new WorldBuffer();

	terrainData = new TerrainData(Contents + L"Landscape/HeightMap.png");


	D3D11_SAMPLER_DESC samplerDesc;
	States::GetSamplerDesc(&samplerDesc);
	States::CreateSampler(&samplerDesc, &diffuseSampler);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	States::CreateSampler(&samplerDesc, &detailSampler);

	ZeroMemory(&textures, sizeof(Texture*) * TEXTURE_MAX);
}

Landscape::GameTerrain::~GameTerrain()
{
	for (UINT i = 0; i < TEXTURE_MAX; i++)
	{
		SAFE_DELETE(textures[i]);
	}

	SAFE_RELEASE(diffuseSampler);
	SAFE_RELEASE(detailSampler);

	SAFE_DELETE(terrainData);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(material);
}

void Landscape::GameTerrain::Render(void)
{
	if (values->renderFlag & (ULONG)ExcuteFlags::Terrain)
	{
		worldBuffer->SetVSBuffer(1);
		material->SetBuffer();

		Texture::SetShaderResources(5, TEXTURE_MAX, textures);

		gDC->PSSetSamplers(0, 1, &diffuseSampler);
		gDC->PSSetSamplers(1, 1, &detailSampler);

		terrainData->Render();
	}
}


float Landscape::GameTerrain::GetVertexHeight(D3DXVECTOR3 position)
{
	return terrainData->GetHeight(position);
}

