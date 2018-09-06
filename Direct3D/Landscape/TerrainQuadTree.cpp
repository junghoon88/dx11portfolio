#include "stdafx.h"
#include "TerrainQuadTree.h"



Landscape::TerrainQuadTree::TerrainQuadTree(wstring file, float heightRatio)
	: heightMapFile(file), heightMap(NULL)
	, width(0), height(0), heightRatio(heightRatio)
	, vertexCount(0)
	, vertexBuffer(NULL)
	, vertexData(NULL)
	, root(NULL)
{
	alphaMapFile[0] = L"";
	alphaMapFile[1] = L"";

	SetHeightMapFile(file, heightRatio);
}

Landscape::TerrainQuadTree::~TerrainQuadTree()
{
	Clear();
}

void Landscape::TerrainQuadTree::SetHeightMapFile(wstring file, float heightRatio)
{
	if (file.length() == 0) 
		return;

	Clear();

	heightMapFile = file;
	heightMap = new Texture(file);

	FillVertexData(heightRatio);

	CreateBuffer();

	root = new QuadTree(0, 0, 0);
	CreateTree(root);
	
	UpdateVertexData(true);

	gCmdManager->ClearCommands();
}

void Landscape::TerrainQuadTree::SetAlphaMapFile(UINT index, wstring file)
{
	assert(index < ALPHAMAP_MAX);

	alphaMapFile[index] = file;
	Texture* alphaMap = new Texture(file);

	DirectX::TexMetadata metaData;
	alphaMap->GetImageInfo(&metaData);

	vector<D3DXCOLOR> pixels;
	D3D11_TEXTURE2D_DESC desc = alphaMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &pixels);
	
	//if (info.Width < width)
	{
		for (UINT i = 0; i < vertexCount; i++)
		{
			UINT ix = i % (width + 1);
			UINT iz = i / (width + 1);

			float tx = (float)ix * (float)metaData.width  / (float)(width + 1);
			float tz = (float)iz * (float)metaData.height / (float)(height + 1);

			//pixel index x,z up,down
			int pixd = (int)(tx);
			int pixu = (int)(ceil(tx));
			int pizd = (int)(tz);
			int pizu = (int)(ceil(tz));

			if (pixu == pixd) pixu = pixd + 1 == metaData.width  ? pixd : pixd + 1;
			if (pizu == pizd) pizu = pizd + 1 == metaData.height ? pizd : pizd + 1;

			D3DXCOLOR color[2];
			//2차 선형 보간
			//X축 기준으로 보간 2번
			D3DXColorLerp(&color[0], &pixels[pixd + pizd * metaData.width], &pixels[pixu + pizd * metaData.width], Math::Fraction(tx));
			D3DXColorLerp(&color[1], &pixels[pixd + pizu * metaData.width], &pixels[pixu + pizu * metaData.width], Math::Fraction(tx));

			//Y축 기준으로 보간 1번
			if (index == 0)	D3DXColorLerp(&vertexData[i].alpha1, &color[0], &color[1], Math::Fraction(tz));
			if (index == 1)	D3DXColorLerp(&vertexData[i].alpha2, &color[0], &color[1], Math::Fraction(tz));
		}
	}

	SAFE_DELETE(alphaMap);

	UpdateVertexData(false);
}

void Landscape::TerrainQuadTree::UpdateVertexData(bool calcNormalData, D3D11_BOX * box)
{
	if (calcNormalData)
	{
		FillNormalData();
	}

	gDC->UpdateSubresource
	(
		vertexBuffer, 0, box, vertexData, sizeof(VertexTypePTNC2) * vertexCount, 0
	);
}


void Landscape::TerrainQuadTree::Render(FrustumCulling* frustumCulling)
{
	UINT stride = sizeof(VertexTypePTNC2);
	UINT offset = 0;

	gDC->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	root->Render(frustumCulling, vertexData);
}

void Landscape::TerrainQuadTree::Clear(void)
{
	SAFE_DELETE(heightMap);

	SAFE_DELETE_ARRAY(vertexData);

	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(root);
}

void Landscape::TerrainQuadTree::FillVertexData(float heightRatio)
{
	vector<D3DXCOLOR> pixels;
	D3D11_TEXTURE2D_DESC desc = heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &pixels);

	width = desc.Width - 1;
	height = desc.Height - 1;


	vertexCount = (width + 1) * (height + 1);
	vertexData = new VertexTypePTNC2[vertexCount];

	UINT index = 0;
	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			vertexData[index].position.x = (float)x;
			vertexData[index].position.y = (float)(pixels[index].r * 255.0f) / heightRatio;
			vertexData[index].position.z = (float)z;

			vertexData[index].uv.x = x / (float)width;
			vertexData[index].uv.y = z / (float)height;

			vertexData[index].normal = D3DXVECTOR3(0, 0, 0);

			index++;
		}//for(x)
	}//for(y)	
}

void Landscape::TerrainQuadTree::FillNormalData(void)
{
	for (UINT i = 0; i < vertexCount; i++)
		vertexData[i].normal = D3DXVECTOR3(0, 0, 0);

	for (UINT i = 0; i < (root->IndexCount / 3); i++)
	{
		UINT index0 = root->IndexData[i * 3 + 0];
		UINT index1 = root->IndexData[i * 3 + 1];
		UINT index2 = root->IndexData[i * 3 + 2];

		VertexTypePTNC2 v0 = vertexData[index0];
		VertexTypePTNC2 v1 = vertexData[index1];
		VertexTypePTNC2 v2 = vertexData[index2];

		D3DXVECTOR3 d1 = v1.position - v0.position;
		D3DXVECTOR3 d2 = v2.position - v0.position;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &d1, &d2);

		vertexData[index0].normal += normal;
		vertexData[index1].normal += normal;
		vertexData[index2].normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertexData[i].normal, &vertexData[i].normal);
}

void Landscape::TerrainQuadTree::CreateBuffer(void)
{
	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTypePTNC2) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertexData;

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Landscape::TerrainQuadTree::CreateTree(QuadTree* tree)
{
	tree->IndexCount = width * height * 6 / pow(4, tree->Level);
	tree->IndexData = new UINT[tree->IndexCount];
	
	UINT xStart = tree->WidthOffset;
	UINT xSize  = UINT(width / pow(2, tree->Level));
	UINT zStart = tree->HeightOffset;
	UINT zSize  = UINT(height / pow(2, tree->Level));

	int index = 0;
	for (UINT z = zStart; z < zStart + zSize; z++)
	{
		for (UINT x = xStart; x < xStart + xSize; x++)
		{
			tree->IndexData[index + 0] = x     + (width + 1) * z;
			tree->IndexData[index + 1] = x     + (width + 1) * (z + 1);
			tree->IndexData[index + 2] = x + 1 + (width + 1) * z;
			tree->IndexData[index + 3] = x + 1 + (width + 1) * z;
			tree->IndexData[index + 4] = x     + (width + 1) * (z + 1);
			tree->IndexData[index + 5] = x + 1 + (width + 1) * (z + 1);

			index += 6;
		}
	}

	//MinMax
	D3DXVECTOR3 MinPos = MathDX::Vector3MaxValue;
	D3DXVECTOR3 MaxPos = MathDX::Vector3MinValue;
	for (UINT i = 0; i < tree->IndexCount; i++)
	{
		D3DXVECTOR3 pos = vertexData[tree->IndexData[i]].position;
		MathDX::GetMinValue(pos, MinPos);
		MathDX::GetMaxValue(pos, MaxPos);
	}
	MinPos.y = 0.0f;
	MaxPos.y = 255.0f / heightRatio;
	tree->box = new BoundingBox(NULL, MinPos, MaxPos);
	tree->box->SetShow(true);

	//IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * tree->IndexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data = { 0 };
		data.pSysMem = tree->IndexData;

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &tree->IndexBuffer);
		assert(SUCCEEDED(hr));
	}


	if (tree->IndexCount > 20000)
	{
		tree->Next[0] = new QuadTree(tree->Level + 1, xStart,			zStart);
		tree->Next[1] = new QuadTree(tree->Level + 1, xStart + xSize/2, zStart);
		tree->Next[2] = new QuadTree(tree->Level + 1, xStart,			zStart + zSize/2);
		tree->Next[3] = new QuadTree(tree->Level + 1, xStart + xSize/2, zStart + zSize/2);

		CreateTree(tree->Next[0]);
		CreateTree(tree->Next[1]);
		CreateTree(tree->Next[2]);
		CreateTree(tree->Next[3]);
	}
}

float Landscape::TerrainQuadTree::GetHeight(D3DXVECTOR3 position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width)
		return 0.0f;

	if (z < 0 || z >= height)
		return 0.0f;

	UINT index0 = (width + 1) * z + x;
	UINT index1 = (width + 1) * (z + 1) + x;
	UINT index2 = (width + 1) * z + x + 1;
	UINT index3 = (width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 v0 = vertexData[index0].position;
	D3DXVECTOR3 v1 = vertexData[index1].position;
	D3DXVECTOR3 v2 = vertexData[index2].position;
	D3DXVECTOR3 v3 = vertexData[index3].position;

	float deltaX = position.x - v0.x;
	float deltaZ = position.z - v0.z;

	D3DXVECTOR3 temp;
	if (deltaX + deltaZ <= 1)
	{
		temp = v0 + (v2 - v0) * deltaX + (v1 - v0) * deltaZ;
	}
	else
	{
		deltaX = 1 - deltaX;
		deltaZ = 1 - deltaZ;

		temp = v3 + (v2 - v3) * deltaX + (v1 - v3) * deltaZ;
	}

	return temp.y;
}

bool Landscape::TerrainQuadTree::MousePickked(D3DXVECTOR3 start, D3DXVECTOR3 direction, OUT D3DXVECTOR2& position)
{
	D3DXMATRIX invWorld;
	D3DXMatrixIdentity(&invWorld);

	D3DXVec3TransformCoord(&start, &start, &invWorld);
	D3DXVec3TransformNormal(&direction, &direction, &invWorld);

	map<float, D3DXVECTOR2> picks;


	//마우스 피킹
	//for (UINT i = 0; i < indexCount; i += 3)
	//{
	//	float u, v, distance;
	//	if (D3DXIntersectTri(&vertexData[indexData[i]].position,
	//		&vertexData[indexData[i + 1]].position,
	//		&vertexData[indexData[i + 2]].position,
	//		&start, &direction, &u, &v, &distance) == TRUE)
	//	{
	//		float minx = vertexData[indexData[i]].position.x;
	//		float minz = vertexData[indexData[i]].position.z;
	//
	//		minx = minx <= vertexData[indexData[i+1]].position.x ? minx : vertexData[indexData[i+1]].position.x;
	//		minz = minz <= vertexData[indexData[i+1]].position.z ? minz : vertexData[indexData[i+1]].position.z;
	//		minx = minx <= vertexData[indexData[i+2]].position.x ? minx : vertexData[indexData[i+2]].position.x;
	//		minz = minz <= vertexData[indexData[i+2]].position.z ? minz : vertexData[indexData[i+2]].position.z;
	//
	//		picks[distance] = D3DXVECTOR2(minx, minz);
	//	}
	//}
	//
	//if (picks.size() > 0)
	//{
	//	position = (*picks.begin()).second;
	//
	//	picks.clear();
	//
	//	return true;
	//}

	return false;
}

void Landscape::TerrainQuadTree::CopyVertexDataTo(OUT VertexTypePTNC2** data)
{
	assert(*data == NULL);

	*data = new VertexTypePTNC2[vertexCount];

	memcpy(*data, vertexData, sizeof(VertexTypePTNC2) * vertexCount);
}

void Landscape::TerrainQuadTree::CopyVertexDataFrom(IN VertexTypePTNC2* data)
{
	memcpy(vertexData, data, sizeof(VertexTypePTNC2) * vertexCount);

	UpdateVertexData(true);
}

void Landscape::TerrainQuadTree::MoveVertexUpDown(D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ, int intensity)
{
	float minY = 0.0f;
	float maxY = 255.0f / heightRatio;

	for (UINT i = 0; i < vertexCount; i++)
	{
		if (vertexData[i].position.x >= startXZ.x && vertexData[i].position.x <= startXZ.x + sizeXZ.x
			&& vertexData[i].position.z >= startXZ.y && vertexData[i].position.z <= startXZ.y + sizeXZ.y)
		{
			vertexData[i].position.y += (float)intensity * heightRatio / 1000.0f;
			if (vertexData[i].position.y < minY)		vertexData[i].position.y = minY;
			else if (vertexData[i].position.y > maxY)	vertexData[i].position.y = maxY;
		}
	}

	UpdateVertexData(true);
}

void Landscape::TerrainQuadTree::MoveVertexUpDown(D3DXVECTOR2 positionXZ, float radius, int intensity)
{
	float minY = 0.0f;
	float maxY = 255.0f / heightRatio;

	float dist = 0.0f;
	for (UINT i = 0; i < vertexCount; i++)
	{
		dist = sqrt(pow(vertexData[i].position.x - positionXZ.x, 2)
				  + pow(vertexData[i].position.z - positionXZ.y, 2));

		if (dist < radius)
		{
			vertexData[i].position.y += ((radius - dist) / radius) * (float)intensity * heightRatio / 1000.0f;
			if (vertexData[i].position.y < minY)		vertexData[i].position.y = minY;
			else if (vertexData[i].position.y > maxY)	vertexData[i].position.y = maxY;
		}
	}

	UpdateVertexData(true);
}

void Landscape::TerrainQuadTree::MoveVertexSmooth(D3DXVECTOR2 positionXZ, float radius, int intensity)
{
	map<UINT, float> mapVertexIndex;

	float dist = 0.0f;
	for (UINT i = 0; i < vertexCount; i++)
	{
		dist = sqrt(pow(vertexData[i].position.x - positionXZ.x, 2)
			+ pow(vertexData[i].position.z - positionXZ.y, 2));

		if (dist < radius)
		{
			mapVertexIndex[i] = 0.0f;
		}
	}

	if (mapVertexIndex.size() == 0)
		return;


	for (auto iter = mapVertexIndex.begin(); iter != mapVertexIndex.end(); ++iter)
	{
		float avrY = 0.0f;
		UINT count = 0;

		UINT idx = iter->first;
		//VertexTypePTNC2 v;

		for (int x = -1; x <= 1; x++)
		{
			//X축 가장자리에 있을 때
			if (x == -1 && idx % (width + 1) == 0)		continue;
			if (x ==  1 && idx % (width + 1) == width)	continue;

			for (int z = -1; z <= 1; z++)
			{
				//Z축 가장자리에 있을 때
				if (z == -1 && idx <= width)				continue;
				if (z ==  1 && idx >= (width + 1)*height)	continue;

				avrY += vertexData[idx + x + z * (width + 1)].position.y;
				count++;
			}
		}
		avrY /= (float)count;

		iter->second = avrY;
	}

	for (auto iter = mapVertexIndex.begin(); iter != mapVertexIndex.end();)
	{
		vertexData[iter->first].position.y = iter->second;
		iter = mapVertexIndex.erase(iter);
	}
	mapVertexIndex.clear();

	UpdateVertexData(true);
}

void Landscape::TerrainQuadTree::AdjustVertexAlpha(UINT selectTexture, D3DXVECTOR2 positionXZ, float radius, int intensity)
{
	if (selectTexture == 0)
		return;

	float dist = 0.0f;
	for (UINT i = 0; i < vertexCount; i++)
	{
		dist = sqrt(pow(vertexData[i].position.x - positionXZ.x, 2)
			+ pow(vertexData[i].position.z - positionXZ.y, 2));

		if (dist < radius)
		{
			float* targetColorValue = NULL;
			switch (selectTexture)
			{
			case (1 << 0):	targetColorValue = &(vertexData[i].alpha1.r);	break;
			case (1 << 1):	targetColorValue = &(vertexData[i].alpha1.g);	break;
			case (1 << 2):	targetColorValue = &(vertexData[i].alpha1.b);	break;
			case (1 << 3):	targetColorValue = &(vertexData[i].alpha2.r);	break;
			case (1 << 4):	targetColorValue = &(vertexData[i].alpha2.g);	break;
			case (1 << 5):	targetColorValue = &(vertexData[i].alpha2.b);	break;
			}

			assert(targetColorValue != NULL);
			*targetColorValue += (float)intensity * 0.001f;
			if (*targetColorValue < 0.0f)			*targetColorValue = 0.0f;
			else if (*targetColorValue > 1.0f)		*targetColorValue = 1.0f;
		}
	}

	UpdateVertexData(false);
}

void Landscape::TerrainQuadTree::AdjustVertexAlpha(UINT selectTexture, D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ, int intensity)
{
	if (selectTexture == 0)
		return;

	float dist = 0.0f;
	for (UINT i = 0; i < vertexCount; i++)
	{
		if (vertexData[i].position.x >= startXZ.x && vertexData[i].position.x <= startXZ.x + sizeXZ.x
			&& vertexData[i].position.z >= startXZ.y && vertexData[i].position.z <= startXZ.y + sizeXZ.y)
		{
			float* targetColorValue = NULL;
			switch (selectTexture)
			{
			case (1 << 0):	targetColorValue = &(vertexData[i].alpha1.r);	break;
			case (1 << 1):	targetColorValue = &(vertexData[i].alpha1.g);	break;
			case (1 << 2):	targetColorValue = &(vertexData[i].alpha1.b);	break;
			case (1 << 3):	targetColorValue = &(vertexData[i].alpha2.r);	break;
			case (1 << 4):	targetColorValue = &(vertexData[i].alpha2.g);	break;
			case (1 << 5):	targetColorValue = &(vertexData[i].alpha2.b);	break;
			}

			assert(targetColorValue != NULL);
			*targetColorValue += (float)intensity * 0.001f;
			if (*targetColorValue < 0.0f)			*targetColorValue = 0.0f;
			else if (*targetColorValue > 1.0f)		*targetColorValue = 1.0f;
		}
	}

	UpdateVertexData(false);
}

void Landscape::TerrainQuadTree::ChangeHeightRatio(float ratio)
{
	for (UINT i = 0; i < vertexCount; i++)
	{
		vertexData[i].position.y *= ratio / heightRatio;
	}

	heightRatio = ratio;
}

void Landscape::TerrainQuadTree::SaveHeightMap(wstring filename)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width + 1;
	desc.Height = height + 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc = { 1, 0 };
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_STAGING; //CPU 접근하려면

	ID3D11Texture2D* texture = NULL;
	HRESULT hr = gDevice->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	UINT* colors = new UINT[vertexCount];

	for (UINT i = 0; i < vertexCount; i++)
	{
		colors[i] = ((UINT)255 << 24)	//a
			+ ((UINT)(vertexData[i].position.y * heightRatio) << 16)	//b
			+ ((UINT)(vertexData[i].position.y * heightRatio) << 8)	//g
			+ ((UINT)(vertexData[i].position.y * heightRatio));		//r
	}

	//리소스 Lock, Unlock 하고 데이터 불러오기
	D3D11_MAPPED_SUBRESOURCE map;
	gDC->Map(texture, 0, D3D11_MAP_WRITE, NULL, &map);
	{
		memcpy(map.pData, colors, sizeof(UINT)*vertexCount);
	}
	gDC->Unmap(texture, 0);

	hr = D3DX11SaveTextureToFile(D3D::GetDC(), texture, D3DX11_IFF_PNG, filename.c_str());
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(colors);
	SAFE_RELEASE(texture);
}

void Landscape::TerrainQuadTree::SaveAlphaMap(UINT index, wstring filename)
{
	assert(index < ALPHAMAP_MAX);

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width + 1;
	desc.Height = height + 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc = { 1, 0 };
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_STAGING; //CPU 접근하려면

	ID3D11Texture2D* texture = NULL;
	HRESULT hr = gDevice->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	UINT* colors = new UINT[vertexCount];

	if (index == 0)
	{
		for (UINT i = 0; i < vertexCount; i++)
		{
			colors[i] = ((UINT)255 << 24)	//a
					+ ((UINT)(vertexData[i].alpha1.r * 255.0f) << 16)	//b
					+ ((UINT)(vertexData[i].alpha1.g * 255.0f) << 8)	//g
					+ ((UINT)(vertexData[i].alpha1.b * 255.0f));		//r
		}
	}
	else
	{
		for (UINT i = 0; i < vertexCount; i++)
		{
			colors[i] = ((UINT)255 << 24)	//a
					+ ((UINT)(vertexData[i].alpha2.r * 255.0f) << 16)	//b
					+ ((UINT)(vertexData[i].alpha2.g * 255.0f) << 8)	//g
					+ ((UINT)(vertexData[i].alpha2.b * 255.0f));		//r
		}
	}

	//리소스 Lock, Unlock 하고 데이터 불러오기
	D3D11_MAPPED_SUBRESOURCE map;
	gDC->Map(texture, 0, D3D11_MAP_WRITE, NULL, &map);
	{
		memcpy(map.pData, colors, sizeof(UINT)*vertexCount);
	}
	gDC->Unmap(texture, 0);

	hr = D3DX11SaveTextureToFile(D3D::GetDC(), texture, D3DX11_IFF_PNG, filename.c_str());
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(colors);
	SAFE_RELEASE(texture);
}

void Landscape::TerrainQuadTree::ClearVertexHeight(void)
{
	for (UINT i = 0; i < vertexCount; i++)
	{
		vertexData[i].position.y = 0.0f;
	}

	UpdateVertexData(true);
}

void Landscape::TerrainQuadTree::ClearVertexAlpha(void)
{
	for (UINT i = 0; i < vertexCount; i++)
	{
		vertexData[i].alpha1 = D3DXCOLOR(0, 0, 0, 0);
		vertexData[i].alpha2 = D3DXCOLOR(0, 0, 0, 0);
	}

	UpdateVertexData(false);
}

void Landscape::TerrainQuadTree::GetTerrainSize(UINT* width, UINT* height)
{
	*width = this->width;
	*height = this->height;
}


void Landscape::TerrainQuadTree::SetIsLanded(bool isLanded, D3DXVECTOR2 grid)
{
	POINT pt;
	pt.x = (int)grid.x;
	pt.y = (int)grid.y;

	if (pt.x < 0 || pt.x > (LONG)width) return;
	if (pt.y < 0 || pt.y > (LONG)height) return;

	int index = pt.x + pt.y * width;
}
