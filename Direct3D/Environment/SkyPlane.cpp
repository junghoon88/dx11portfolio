#include "stdafx.h"
#include "SkyPlane.h"


Environment::SkyPlane::SkyPlane(wstring cloudFile, wstring perturbFile)
	: bShowWireFrame(false)
{
	skyPlaneResolution = 50;
	skyPlaneWidth = 10.0f;
	skyPlaneTop = 0.5f;
	skyPlaneBottom = 0.0f;
	textureRepeat = 2;
	

	// 하늘 평면을 만듭니다.
	CreateSkyPlane();

	// 클라우드 텍스처 객체를 생성합니다.
	cloudTexture = new Texture(cloudFile);

	// 섭동 텍스처 오브젝트를 생성합니다.
	perturbTexture = new Texture(perturbFile);


	//쉐이더, 버퍼 생성
	shader = new Shader(Shaders + L"Skyplane.hlsl");
	skyPlaneBuffer = new SkyPlaneBuffer();

	CreateStates();


}


Environment::SkyPlane::~SkyPlane()
{
	// 하늘 평면 텍스처를 해제합니다.
	SAFE_DELETE(perturbTexture);
	SAFE_DELETE(cloudTexture);

	// 하늘 평면 렌더링에 사용 된 정점 및 인덱스 버퍼를 해제합니다.
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(skyPlaneBuffer);


	// 샘플러 상태를 해제한다.
	SAFE_RELEASE(rasterizerState[0]);
	SAFE_RELEASE(rasterizerState[1]);

	SAFE_RELEASE(defaultSampleState);

	SAFE_RELEASE(blendState[0]);
	SAFE_RELEASE(blendState[1]);
}

void Environment::SkyPlane::LoadData(Json::Value * value)
{
	Json::Value skyPlane = (*value)["Environment"]["SkyPlane"];

	if (skyPlane.isNull() == false)
	{
		Json::GetValue(skyPlane, "skyPlaneResolution",	skyPlaneResolution);
		Json::GetValue(skyPlane, "skyPlaneWidth",		skyPlaneWidth);
		Json::GetValue(skyPlane, "skyPlaneTop",			skyPlaneTop);
		Json::GetValue(skyPlane, "skyPlaneBottom",		skyPlaneBottom);
		Json::GetValue(skyPlane, "textureRepeat",		textureRepeat);
		Json::GetValue(skyPlane, "scale",				skyPlaneBuffer->Data.scale);
		Json::GetValue(skyPlane, "brightness",			skyPlaneBuffer->Data.brightness);
	}
}

void Environment::SkyPlane::SaveData(Json::Value * value)
{
	Json::Value skyPlane;

	Json::SetValue(skyPlane, "skyPlaneResolution",	skyPlaneResolution);
	Json::SetValue(skyPlane, "skyPlaneWidth",		skyPlaneWidth);
	Json::SetValue(skyPlane, "skyPlaneTop",			skyPlaneTop);
	Json::SetValue(skyPlane, "skyPlaneBottom",		skyPlaneBottom);
	Json::SetValue(skyPlane, "textureRepeat",		textureRepeat);
	Json::SetValue(skyPlane, "scale",				skyPlaneBuffer->Data.scale);
	Json::SetValue(skyPlane, "brightness",			skyPlaneBuffer->Data.brightness);

	(*value)["Environment"]["SkyPlane"] = skyPlane;
}

void Environment::SkyPlane::Update(void)
{
	// 각 프레임의 텍스처 변환 값을 증가시킵니다.
	float delta = 0.0001f;
	skyPlaneBuffer->Data.translation = Math::Modulo(skyPlaneBuffer->Data.translation + delta, 1.0f);
}

void Environment::SkyPlane::Render(void)
{
	if (vertexBuffer == NULL || indexBuffer == NULL)
		return;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	gDC->OMSetBlendState(blendState[1], blendFactor, 0xffffffff);

	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexTexture);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	gDC->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	gDC->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	gDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	// 렌더링에 사용할 셰이더 매개 변수를 설정합니다.
	skyPlaneBuffer->SetPSBuffer(2);

	// 픽셀 쉐이더에서 쉐이더 텍스처 리소스를 설정합니다.
	cloudTexture->SetShaderResource(5);
	perturbTexture->SetShaderResource(6);

	// 설정된 버퍼를 셰이더로 렌더링한다.
	shader->Render();

	if (bShowWireFrame)
	{
		gDC->RSSetState(rasterizerState[1]);
	}


	// 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
	gDC->PSSetSamplers(5, 1, &defaultSampleState);

	// 삼각형을 그립니다.
	gDC->DrawIndexed(indexCount, 0, 0);

	//블랜드 끄기
	gDC->OMSetBlendState(blendState[0], blendFactor, 0xffffffff);

	gDC->RSSetState(rasterizerState[0]);
}

void Environment::SkyPlane::PostRender(void)
{
	ImGui::Text("SkyPlane");

	ImGui::Checkbox("Show WireFrame", &bShowWireFrame);

	bool changed = false;
	changed |= ImGui::DragInt("skyPlaneResolution", &skyPlaneResolution, 0.1f, 1, 100);
	changed |= ImGui::DragFloat("skyPlaneWidth", &skyPlaneWidth, 0.1f, 0.01f, 100.0f);
	changed |= ImGui::DragFloat("skyPlaneTop", &skyPlaneTop, 0.01f, 0.0f, 1.0f);
	changed |= ImGui::DragFloat("skyPlaneBottom", &skyPlaneBottom, 0.01f, 0.0f, 1.0f);
	changed |= ImGui::DragInt("textureRepeat", &textureRepeat, 0.01f, 1, 10);

	ImGui::DragFloat("scale", &skyPlaneBuffer->Data.scale, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("brightness", &skyPlaneBuffer->Data.brightness, 0.01f, 0.0f, 1.0f);

	if (changed)
	{
		ClearSkyPlane();
		CreateSkyPlane();
	}
}

void Environment::SkyPlane::CreateSkyPlane(void)
{
	// 하늘 평면 좌표를 보유 할 배열을 만듭니다.
	SkyPlaneType* skyPlaneData = new SkyPlaneType[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];

	// 하늘 평면에서 각 쿼드의 크기를 결정합니다.
	float quadSize = skyPlaneWidth / (float)skyPlaneResolution;

	// 너비를 기준으로 하늘 평면의 반지름을 계산합니다.
	float radius = skyPlaneWidth / 2.0f;

	// 증가 할 높이 상수를 계산합니다.
	float constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	// 텍스처 좌표 증가 값을 계산합니다.
	float textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

	float positionX, positionY, positionZ;
	float tu, tv;

	// 하늘 평면을 반복하고 제공된 증분 값을 기반으로 좌표를 만듭니다.
	for (int j = 0; j <= skyPlaneResolution; j++)
	{
		for (int i = 0; i <= skyPlaneResolution; i++)
		{
			// 정점 좌표를 계산합니다.
			positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

			// 텍스처 좌표를 계산합니다.
			tu = (float)i * textureDelta;
			tv = (float)j * textureDelta;

			// 이 좌표를 추가하기 위해 하늘 평면 배열에 인덱스를 계산합니다.
			int index = j * (skyPlaneResolution + 1) + i;

			// 하늘 평면 배열에 좌표를 추가합니다.
			skyPlaneData[index].pos = { positionX, positionY, positionZ };
			skyPlaneData[index].uv = { tu, tv };
		}
	}

	// 하늘 평면 메쉬의 정점 수를 계산합니다.
	vertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;

	// 인덱스 수를 꼭지점 수와 같게 설정합니다.
	indexCount = vertexCount;
		
	// 정점 배열을 만듭니다.
	VertexTexture* vertices = new VertexTexture[vertexCount];
	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[indexCount];

	// 인덱스를 정점 배열로 초기화 합니다.
	int index = 0;
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index4 = 0;

	// 하늘 평면 배열 데이터로 꼭지점과 인덱스 배열을 로드합니다.
	for(int j=0; j<skyPlaneResolution; j++)
	{
		for(int i=0; i<skyPlaneResolution; i++)
		{
			index1 = j * (skyPlaneResolution + 1) + i;
			index2 = j * (skyPlaneResolution + 1) + (i+1);
			index3 = (j+1) * (skyPlaneResolution + 1) + i;
			index4 = (j+1) * (skyPlaneResolution + 1) + (i+1);

			// 삼각형 1 - 왼쪽 위
			vertices[index].position = skyPlaneData[index1].pos;
			vertices[index].uv = skyPlaneData[index1].uv;
			indices[index] = index;
			index++;

			// 삼각형 1 - 오른쪽 위
			vertices[index].position = skyPlaneData[index2].pos;
			vertices[index].uv = skyPlaneData[index2].uv;
			indices[index] = index;
			index++;

			// 삼각형 1 - 왼쪽 아래
			vertices[index].position = skyPlaneData[index3].pos;
			vertices[index].uv = skyPlaneData[index3].uv;
			indices[index] = index;
			index++;

			// 삼각형 2 - 왼쪽 아래
			vertices[index].position = skyPlaneData[index3].pos;
			vertices[index].uv = skyPlaneData[index3].uv;
			indices[index] = index;
			index++;

			// 삼각형 2 - 오른쪽 위
			vertices[index].position = skyPlaneData[index2].pos;
			vertices[index].uv = skyPlaneData[index2].uv;
			indices[index] = index;
			index++;

			// 삼각형 2 - 오른쪽 아래
			vertices[index].position = skyPlaneData[index4].pos;
			vertices[index].uv = skyPlaneData[index4].uv;
			indices[index] = index;
			index++;
		}
	}

	//Vertex
	{
		// 정점 버퍼의 구조체를 설정한다.
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexTexture) * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// 정점 버퍼를 만듭니다.
		HRESULT hr;
		hr = gDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//Index
	{
		// 인덱스 버퍼의 구조체를 설정합니다.
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// 인덱스 버퍼를 만듭니다.
		HRESULT hr;
		hr = gDevice->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

	// 버텍스와 인덱스 버퍼가 생성되고 로드된 배열을 해제합니다.
	SAFE_DELETE_ARRAY(skyPlaneData);
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void Environment::SkyPlane::ClearSkyPlane(void)
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);
}

void Environment::SkyPlane::CreateStates(void)
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	States::GetRasterizerDesc(&rasterizerDesc);
	States::CreateRasterizer(&rasterizerDesc, &rasterizerState[0]);
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	States::CreateRasterizer(&rasterizerDesc, &rasterizerState[1]);



	D3D11_SAMPLER_DESC samplerDesc;
	States::GetSamplerDesc(&samplerDesc);
	States::CreateSampler(&samplerDesc, &defaultSampleState);



	D3D11_BLEND_DESC blendDesc;
	States::GetBlendDesc(&blendDesc);
	States::CreateBlend(&blendDesc, &blendState[0]);
	States::GetBlendDesc(&blendDesc, 0, BlendMethod::AddTexture12);
	States::CreateBlend(&blendDesc, &blendState[1]);
}


