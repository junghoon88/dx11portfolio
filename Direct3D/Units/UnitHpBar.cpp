#include "stdafx.h"
#include "UnitHpBar.h"
#include "GameUnit.h"

UnitHpBar::UnitHpBar(GameUnit* unit)
	: unit(unit)
{
	/*
	0---1
	2---3
	*/
	shader = new Shader(Shaders + L"UnitHpBar.hlsl");


	vertexData = new VertexTexture[4];
	vertexData[0].position = D3DXVECTOR3(0, 0, 0);	vertexData[0].uv = D3DXVECTOR2(0, 0);
	vertexData[1].position = D3DXVECTOR3(0, 0, 0);	vertexData[1].uv = D3DXVECTOR2(1, 0);
	vertexData[2].position = D3DXVECTOR3(0, 0, 0);	vertexData[2].uv = D3DXVECTOR2(0, 1);
	vertexData[3].position = D3DXVECTOR3(0, 0, 0);	vertexData[3].uv = D3DXVECTOR2(1, 1);

	indexData = new UINT[6];
	indexData[0] = 0;
	indexData[1] = 2;
	indexData[2] = 1;
	indexData[3] = 1;
	indexData[4] = 2;
	indexData[5] = 3;

	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertexData;

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data = { 0 };
		data.pSysMem = indexData;

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

	buffer = new Buffer();

	texture[0] = new Texture(Textures + L"hpBar.bmp");
	texture[1] = new Texture(Textures + L"hpBar2.bmp");

}

UnitHpBar::~UnitHpBar()
{
	SAFE_DELETE(buffer);
	SAFE_DELETE(texture[0]);
	SAFE_DELETE(texture[1]);

	SAFE_DELETE(vertexData);
	SAFE_DELETE(indexData);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void UnitHpBar::Update(float hpRatio)
{
	if (unit == NULL)
		return;

	D3DXMATRIX mat, root, world;
	unit->GetRootAxis(root);
	unit->GetWorld(world);
	mat = root * world;

	D3DXVECTOR3 zero(0, 0, 0);
	for (size_t i = 0; i < 4; i++)
	{
		D3DXVec3TransformCoord(&vertexData[i].position, &zero, &mat);
	}

	gDC->UpdateSubresource
	(
		vertexBuffer, 0, NULL, vertexData, sizeof(VertexTexture) * 4, 0
	);

	buffer->Data.hpRatio = hpRatio;


}

void UnitHpBar::Render(void)
{
	if (buffer->Data.hpRatio == 0.0f)
		return;

	shader->Render();

	buffer->SetVSBuffer(5);

	Texture::SetShaderResources(5, 2, texture);

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;


	gDC->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDC->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gDC->DrawIndexed(6, 0, 0);
}
