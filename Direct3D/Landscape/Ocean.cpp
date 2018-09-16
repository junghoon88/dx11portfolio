#include "stdafx.h"
#include "Ocean.h"

Landscape::Ocean::Ocean(ExecuteValues* values, UINT width, UINT height, UINT thick,
						UINT xOffset, UINT yOffset)
	: values(values)
	, vertexBuffer(NULL), indexBuffer(NULL)
	, vertices(NULL), indices(NULL)
	, width(width), height(height), thick(thick)
	, xOffset(xOffset), yOffset(yOffset)
	, bWireframe(false)
	, heightMap(NULL)
{
	material = new Material(Shaders + L"Ocean.hlsl");
	material->SetDiffuse(D3DXCOLOR(0.6f, 0.721f, 0.937f, 0.627f));
	material->SetNormalMap(Contents + L"Landscape/WaveNormal.png");
	material->SetShininess(500);

	worldBuffer = new WorldBuffer();

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

	CreateVertex();
}

Landscape::Ocean::~Ocean()
{
	SAFE_DELETE(material);
	SAFE_DELETE(worldBuffer);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);


	SAFE_RELEASE(rasterizer[0]);
	SAFE_RELEASE(rasterizer[1]);
}

void Landscape::Ocean::Update(void)
{
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, 0, 3, 0);
	worldBuffer->SetMatrix(T);
}

void Landscape::Ocean::Render(void)
{
	if (values->renderFlag & (ULONG)ExcuteFlags::Ocean)
	{
		vsBuffer->SetVSBuffer(2);
		psBuffer->SetPSBuffer(2);

		worldBuffer->SetVSBuffer(1);

		material->SetBuffer();

		if (heightMap)
		{
			heightMap->SetShaderResource(5);
			heightMap->SetShaderSampler(5);
		}

		if (bWireframe == true)
			D3D::GetDC()->RSSetState(rasterizer[1]);

		D3D::GetDC()->OMSetBlendState(blendState[1], NULL, 0xff);

		{
			UINT stride = sizeof(OceanVertexType);
			UINT offset = 0;

			D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
		}

		D3D::GetDC()->RSSetState(rasterizer[0]);
		D3D::GetDC()->OMSetBlendState(blendState[0], NULL, 0xff);
	}
}

void Landscape::Ocean::PostRender(void)
{
	if (ImGui::Begin("Ocean"))
	{
		ImGui::InputInt("width",   (int*)&width);
		ImGui::InputInt("height",  (int*)&height);
		ImGui::InputInt("thick",   (int*)&thick);
		ImGui::InputInt("xOffset", (int*)&xOffset);
		ImGui::InputInt("yOffset", (int*)&yOffset);

		if (ImGui::Button("Reset"))
		{
			CreateVertex();
		}

		ImGui::Separator();
		//VSBuffer
		ImGui::InputFloat("WaveFrequency",  &vsBuffer->Data.WaveFrequency);
		ImGui::InputFloat("WaveAmplitude",  &vsBuffer->Data.WaveAmplitude);
		ImGui::InputFloat2("TextureScale",	&vsBuffer->Data.TextureScale[0]);
		ImGui::InputFloat2("BumpSpeed",		&vsBuffer->Data.BumpSpeed[0]);
		ImGui::InputFloat("BumpHeight",		&vsBuffer->Data.BumpHeight);

		ImGui::Separator();
		//PSBuffer
		ImGui::ColorEdit3("ShallowColor",	&psBuffer->Data.ShallowColor[0]);
		ImGui::ColorEdit3("DeepColor",		&psBuffer->Data.DeepColor[0]);
		ImGui::InputFloat("FresnelBias",	&psBuffer->Data.FresnelBias);
		ImGui::InputFloat("FresnelPower",	&psBuffer->Data.FresnelPower);
		ImGui::InputFloat("FresnelAmount",	&psBuffer->Data.FresnelAmount);
		ImGui::InputFloat("SpecularAmount", &psBuffer->Data.SpecularAmount);
		ImGui::InputFloat("HeightRatio",	&psBuffer->Data.HeightRatio);
		ImGui::InputFloat("ShoreBlend",		&psBuffer->Data.ShoreBlend);
	}
	ImGui::End();
}

void Landscape::Ocean::CreateVertex(void)
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	//VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new OceanVertexType[vertexCount];

		UINT index = 0;
		for (UINT y = 0; y <= height; y++)
		{
			for (UINT x = 0; x <= width; x++)
			{
				vertices[index].position.x = (float)(x * thick + xOffset);
				vertices[index].position.y = 0.0f;
				vertices[index].position.z = (float)(y * thick + yOffset);

				vertices[index].uv.x = (float)x / (float)width;
				vertices[index].uv.y = (float)y / (float)width;

				index++;
			}
		}
	}

	//IndexData
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];
		UINT index = 0;
		for (UINT z = 0; z < height; z++)
		{
			for (UINT x = 0; x < width; x++)
			{
				indices[index + 0] = x     + (width + 1) * z;
				indices[index + 1] = x     + (width + 1) * (z + 1);
				indices[index + 2] = x + 1 + (width + 1) * z;
				indices[index + 3] = x + 1 + (width + 1) * z;
				indices[index + 4] = x     + (width + 1) * (z + 1);
				indices[index + 5] = x + 1 + (width + 1) * (z + 1);

				index += 6;
			}
		}
	}


	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(OceanVertexType) * vertexCount;
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
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

	psBuffer->Data.OceanSize = D3DXVECTOR2(float(width * thick), float(height * thick));
}

void Landscape::Ocean::LoadData(Json::Value * value)
{
	Json::Value ocean = (*value)["Landscape"]["Ocean"];
	if (ocean.isNull() == false)
	{
		Json::GetValue(ocean, "Width",		width);
		Json::GetValue(ocean, "Height",		height);
		Json::GetValue(ocean, "Thick",		thick);
		Json::GetValue(ocean, "XOffset",	xOffset);
		Json::GetValue(ocean, "YOffset",	yOffset);

		//VSBuffer
		Json::GetValue(ocean, "WaveFrequency",	vsBuffer->Data.WaveFrequency);
		Json::GetValue(ocean, "WaveAmplitude",	vsBuffer->Data.WaveAmplitude);
		Json::GetValue(ocean, "TextureScale",	vsBuffer->Data.TextureScale);
		Json::GetValue(ocean, "BumpSpeed",		vsBuffer->Data.BumpSpeed);
		Json::GetValue(ocean, "BumpHeight",		vsBuffer->Data.BumpHeight);

		//PSBuffer
		Json::GetValue(ocean, "ShallowColor",	psBuffer->Data.ShallowColor);
		Json::GetValue(ocean, "DeepColor",		psBuffer->Data.DeepColor);
		Json::GetValue(ocean, "FresnelBias",	psBuffer->Data.FresnelBias);
		Json::GetValue(ocean, "FresnelPower",	psBuffer->Data.FresnelPower);
		Json::GetValue(ocean, "FresnelAmount",	psBuffer->Data.FresnelAmount);
		Json::GetValue(ocean, "SpecularAmount", psBuffer->Data.SpecularAmount);
		Json::GetValue(ocean, "HeightRatio",	psBuffer->Data.HeightRatio);
		Json::GetValue(ocean, "ShoreBlend",		psBuffer->Data.ShoreBlend);

		CreateVertex();
	}
}

void Landscape::Ocean::SaveData(Json::Value * value)
{
	Json::Value ocean;

	Json::SetValue(ocean, "Width",   width);
	Json::SetValue(ocean, "Height",  height);
	Json::SetValue(ocean, "Thick",   thick);
	Json::SetValue(ocean, "XOffset", xOffset);
	Json::SetValue(ocean, "YOffset", yOffset);

	//VSBuffer
	Json::SetValue(ocean, "WaveFrequency",  vsBuffer->Data.WaveFrequency);
	Json::SetValue(ocean, "WaveAmplitude",  vsBuffer->Data.WaveAmplitude);
	Json::SetValue(ocean, "TextureScale",	vsBuffer->Data.TextureScale);
	Json::SetValue(ocean, "BumpSpeed",		vsBuffer->Data.BumpSpeed);
	Json::SetValue(ocean, "BumpHeight",		vsBuffer->Data.BumpHeight);

	//PSBuffer
	Json::SetValue(ocean, "ShallowColor",	psBuffer->Data.ShallowColor);
	Json::SetValue(ocean, "DeepColor",		psBuffer->Data.DeepColor);
	Json::SetValue(ocean, "FresnelBias",	psBuffer->Data.FresnelBias);
	Json::SetValue(ocean, "FresnelPower",	psBuffer->Data.FresnelPower);
	Json::SetValue(ocean, "FresnelAmount",	psBuffer->Data.FresnelAmount);
	Json::SetValue(ocean, "SpecularAmount", psBuffer->Data.SpecularAmount);
	Json::SetValue(ocean, "HeightRatio",	psBuffer->Data.HeightRatio);
	Json::SetValue(ocean, "ShoreBlend",		psBuffer->Data.ShoreBlend);

	(*value)["Landscape"]["Ocean"] = ocean;
}
