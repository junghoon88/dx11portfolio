#include "stdafx.h"
#include "Ocean.h"

Ocean::Ocean(ExecuteValues* values, UINT width, UINT height, float thick, Texture* heightMap)
	: values(values)
	, vertexBuffer(NULL), indexBuffer(NULL)
	, width(width), height(height), thick(thick)
	, bWireframe(false)
	, heightMap(heightMap)
{
	material = new Material(Shaders + L"998_Ocean.hlsl");
	material->SetDiffuse(D3DXCOLOR(0.6f, 0.721f, 0.937f, 0.627f));
	material->SetNormalMap(Contents + L"Landscape/WaveNormal.png");
	material->SetShininess(500);

	worldBuffer = new WorldBuffer();

	UINT count = (width + 1) * (height + 1);
	vertices.assign(count, OceanVertexType());

	UINT index = 0;
	for (UINT y = 0; y <= height; y++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			vertices[index].position.x = (float)x * thick;
			vertices[index].position.y = 0.0f;
			vertices[index].position.z = (float)y * thick;

			vertices[index].uv.x = (float)x / (float)width;
			vertices[index].uv.y = (float)y / (float)width;

			index++;
		}
	}

	UINT indexCount = width * height * 6;
	indices.assign(indexCount, UINT());
	index = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			indices[index + 0] = (width + 1) * z + x;
			indices[index + 1] = (width + 1) * (z + 1) + x;
			indices[index + 2] = (width + 1) * z + x + 1;
			indices[index + 3] = (width + 1) * z + x + 1;
			indices[index + 4] = (width + 1) * (z + 1) + x;
			indices[index + 5] = (width + 1) * (z + 1) + (x + 1);

			index += 6;
		}
	}


	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(OceanVertexType) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertices[0];

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc = { 0 };
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		States::CreateRasterizer(&desc, &rasterizer[0]);

		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &rasterizer[1]);
	}
	{
		D3D11_BLEND_DESC desc;
		States::GetBlendDesc(&desc);
		States::CreateBlend(&desc, &blendState[0]);

		desc.RenderTarget[0].BlendEnable = true;
		States::CreateBlend(&desc, &blendState[1]);

	}

	vsBuffer = new VSBuffer();
	psBuffer = new PSBuffer();
	psBuffer->Data.OceanSize = D3DXVECTOR2(width * thick, height * thick);
}

Ocean::~Ocean()
{
	SAFE_DELETE(material);
	SAFE_DELETE(worldBuffer);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_RELEASE(rasterizer[0]);
	SAFE_RELEASE(rasterizer[1]);
}

void Ocean::Update(void)
{
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, 0, 3, 0);
	worldBuffer->SetMatrix(T);
}

void Ocean::Render(void)
{
	vsBuffer->SetVSBuffer(2);
	psBuffer->SetPSBuffer(2);

	worldBuffer->SetVSBuffer(1);

	material->SetBuffer();

	heightMap->SetShaderResource(5);
	heightMap->SetShaderSampler(5);

	if (bWireframe == true)
		D3D::GetDC()->RSSetState(rasterizer[1]);

	D3D::GetDC()->OMSetBlendState(blendState[1], NULL, 0xff);

	{
		UINT stride = sizeof(OceanVertexType);
		UINT offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D::GetDC()->DrawIndexed(indices.size(), 0, 0);
	}

	D3D::GetDC()->RSSetState(rasterizer[0]);
	D3D::GetDC()->OMSetBlendState(blendState[0], NULL, 0xff);
}
