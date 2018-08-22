#include "stdafx.h"
#include "Grid3Axis.h"

Grid3Axis::Grid3Axis(int gridSize)
	: gridSize(gridSize)
	, vertexBuffer(NULL), indexBuffer(NULL)
	, vertexData(NULL)
{
	CreateVertex();
	shader = new Shader(Shaders + L"Grid3Axis.hlsl");

	worldBuffer = new WorldBuffer();
}

Grid3Axis::~Grid3Axis()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_DELETE_ARRAY(vertexData);

	SAFE_DELETE(worldBuffer);
}


void Grid3Axis::Render(void)
{
	shader->Render();

	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	worldBuffer->SetVSBuffer(1);

	gDC->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDC->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	gDC->DrawIndexed(indexCount, 0, 0);
}


void Grid3Axis::CreateVertex(void)
{
	vertexCount = (gridSize * 2 + 1) * 3 * 4; //사이즈*2+1, 3축, 직선에 필요한 2개
	vertexData = new VertexColor[vertexCount];

	UINT index = 0;
	//기준선
	{
		vertexData[index].position.x = -gridSize;
		vertexData[index].position.y = 0;
		vertexData[index].position.z = 0;
		vertexData[index].color = D3DXCOLOR(1, 0, 0, 1);
		index++;

		vertexData[index].position.x = gridSize;
		vertexData[index].position.y = 0;
		vertexData[index].position.z = 0;
		vertexData[index].color = D3DXCOLOR(1, 0, 0, 1);
		index++;

		vertexData[index].position.x = 0;
		vertexData[index].position.y = -gridSize;
		vertexData[index].position.z = 0;
		vertexData[index].color = D3DXCOLOR(0, 1, 0, 1);
		index++;

		vertexData[index].position.x = 0;
		vertexData[index].position.y = gridSize;
		vertexData[index].position.z = 0;
		vertexData[index].color = D3DXCOLOR(0, 1, 0, 1);
		index++;

		vertexData[index].position.x = 0;
		vertexData[index].position.y = 0;
		vertexData[index].position.z = -gridSize;
		vertexData[index].color = D3DXCOLOR(0, 0, 1, 1);
		index++;

		vertexData[index].position.x = 0;
		vertexData[index].position.y = 0;
		vertexData[index].position.z = gridSize;
		vertexData[index].color = D3DXCOLOR(0, 0, 1, 1);
		index++;
	}


	D3DXCOLOR color;
	UINT xCount = 0;
	//X축
	for (int x = -gridSize; x <= gridSize; x++)
	{
		if (x == 0) continue;

		color = (x % 10 == 0) ? D3DXCOLOR(1, 1, 1, 1) : D3DXCOLOR(0.3f, 0.3f, 0.3f, 1);
	
		vertexData[index].position.x = x;
		vertexData[index].position.y = -gridSize;
		vertexData[index].position.z = 0;
		vertexData[index].color = color;
		index++;
	
		vertexData[index].position.x = x;
		vertexData[index].position.y = gridSize;
		vertexData[index].position.z = 0;
		vertexData[index].color = color;
		index++;
	
		vertexData[index].position.x = x;
		vertexData[index].position.y = 0;
		vertexData[index].position.z = -gridSize;
		vertexData[index].color = color;
		index++;
	
		vertexData[index].position.x = x;
		vertexData[index].position.y = 0;
		vertexData[index].position.z = gridSize;
		vertexData[index].color = color;
		index++;

		xCount += 4;
	}

	//Y축
	for (int y = -gridSize; y <= gridSize; y++)
	{
		if (y == 0) continue;
		
		color = (y % 10 == 0) ? D3DXCOLOR(1, 1, 1, 1) : D3DXCOLOR(0.3f, 0.3f, 0.3f, 1);

		vertexData[index].position.x = -gridSize;
		vertexData[index].position.y = y;
		vertexData[index].position.z = 0;
		vertexData[index].color = color;
		index++;

		vertexData[index].position.x = gridSize;
		vertexData[index].position.y = y;
		vertexData[index].position.z = 0;
		vertexData[index].color = color;
		index++;

		vertexData[index].position.x = 0;
		vertexData[index].position.y = y;
		vertexData[index].position.z = -gridSize;
		vertexData[index].color = color;
		index++;

		vertexData[index].position.x = 0;
		vertexData[index].position.y = y;
		vertexData[index].position.z = gridSize;
		vertexData[index].color = color;
		index++;
	}

	//Y축
	for (int z = -gridSize; z <= gridSize; z++)
	{
		if (z == 0) continue;

		color = (z % 10 == 0) ? D3DXCOLOR(1, 1, 1, 1) : D3DXCOLOR(0.3f, 0.3f, 0.3f, 1);

		vertexData[index].position.x = -gridSize;
		vertexData[index].position.y = 0;
		vertexData[index].position.z = z;
		vertexData[index].color = color;
		index++;

		vertexData[index].position.x = gridSize;
		vertexData[index].position.y = 0;
		vertexData[index].position.z = z;
		vertexData[index].color = color;
		index++;

		vertexData[index].position.x = 0;
		vertexData[index].position.y = -gridSize;
		vertexData[index].position.z = z;
		vertexData[index].color = color;
		index++;

		vertexData[index].position.x = 0;
		vertexData[index].position.y = gridSize;
		vertexData[index].position.z = z;
		vertexData[index].color = color;
		index++;
	}

	indexCount = vertexCount;
	indexData = new UINT[indexCount];
	for (UINT i = 0; i < indexCount; i++)
	{
		indexData[i] = i;
	}



	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertexCount;
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
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data = { 0 };
		data.pSysMem = indexData;

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}
