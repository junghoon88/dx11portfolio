#include "stdafx.h"
#include "AStarViewer.h"

#include "TerrainData.h"

AStarViewer::AStarViewer()
	: vertexBuffer(NULL), indexBuffer(NULL)
{
	shader = new Shader(Shaders + L"AStarViewer.hlsl");

	worldBuffer = new WorldBuffer();
}

AStarViewer::~AStarViewer()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(worldBuffer);

	SAFE_DELETE(shader);
}

void AStarViewer::CreateBuffer(vTile tiles, D3DXCOLOR color)
{
	vertexData.clear();
	indexData.clear();
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	VertexColor vertex;
	vertex.color = color;

	D3DXVECTOR3 pos[4];
	for (UINT i = 0; i < tiles.size(); i++)
	{
		pos[0] = D3DXVECTOR3(float(tiles[i]->idX    ), tiles[i]->AvgHeight, float(tiles[i]->idY    ));
		pos[1] = D3DXVECTOR3(float(tiles[i]->idX    ), tiles[i]->AvgHeight, float(tiles[i]->idY + 1));
		pos[2] = D3DXVECTOR3(float(tiles[i]->idX + 1), tiles[i]->AvgHeight, float(tiles[i]->idY    ));
		pos[3] = D3DXVECTOR3(float(tiles[i]->idX + 1), tiles[i]->AvgHeight, float(tiles[i]->idY + 1));


		vertex.position = pos[0]; vertexData.push_back(vertex);
		vertex.position = pos[1]; vertexData.push_back(vertex);
		vertex.position = pos[2]; vertexData.push_back(vertex);
		vertex.position = pos[3]; vertexData.push_back(vertex);


		indexData.push_back(i * 4 + 0);
		indexData.push_back(i * 4 + 1);
		indexData.push_back(i * 4 + 2);
		indexData.push_back(i * 4 + 2);
		indexData.push_back(i * 4 + 1);
		indexData.push_back(i * 4 + 3);
	}


	//버텍스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertexData.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertexData[0];

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//인덱스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexData.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indexData[0];

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}


void AStarViewer::Render(void)
{
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	//IA : InputAssembly
	gDC->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDC->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);
	shader->Render();


	gDC->DrawIndexed(indexData.size(), 0, 0);
}


