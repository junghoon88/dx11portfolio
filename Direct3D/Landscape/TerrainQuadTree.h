#pragma once

#include "../Model/Bounding/BoundingBox.h"
#include "../Model/Bounding/BoundingFrustum.h"
#include "../Model/Bounding/Ray.h"

#define ALPHAMAP_MAX	2


struct QuadTree
{
	int				Level;
	UINT			WidthOffset;
	UINT			HeightOffset;

	UINT			VertexCount;
	ID3D11Buffer*	VertexBuffer;
	TerrainVertexType* VertexData;

	UINT			IndexCount;
	ID3D11Buffer*	IndexBuffer;
	UINT*			IndexData;
	BoundingBox*	box;
	QuadTree*		Next[4];

	QuadTree(int level, UINT widthOffset, UINT heightOffset)
	{
		Level = level;
		WidthOffset = widthOffset;
		HeightOffset = heightOffset;

		VertexCount = 0;
		VertexBuffer = NULL;
		VertexData = NULL;

		IndexCount = 0;
		IndexBuffer = NULL;
		IndexData = NULL;
		box = NULL;

		Next[0] = NULL;
		Next[1] = NULL;
		Next[2] = NULL;
		Next[3] = NULL;
	}
	~QuadTree()
	{
		SAFE_DELETE(box);

		SAFE_RELEASE(VertexBuffer);
		SAFE_DELETE_ARRAY(VertexData);

		SAFE_RELEASE(IndexBuffer);
		SAFE_DELETE_ARRAY(IndexData);

		for (size_t i = 0; i < 4; i++)
			SAFE_DELETE(Next[i]);
	}

	void Render(BoundingFrustum* frustum)
	{
		bool bContain = false;
		ContainmentType type = frustum->Contains(box);
		if (type == ContainmentType::Contains
			|| type == ContainmentType::Intersects)
		{
			bContain = true;
		}

		//if (bContain)
		{
			bool bNext = false;
			for (int i = 0; i < 4; i++)
			{
				if (Next[i] != NULL)
				{
					bNext = true;
					Next[i]->Render(frustum);
				}
			}

			if (bNext == false)
			{
				UINT stride = sizeof(TerrainVertexType);
				UINT offset = 0;
				gDC->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
				gDC->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
				gDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				gDC->DrawIndexed(IndexCount, 0, 0);
			}
		}
	}

	bool MousePickked(D3DXVECTOR3& start, D3DXVECTOR3& direction, OUT D3DXVECTOR2& position)
	{
		float u, v, distance;


		if (Next[0] != NULL)
		{
			//하위 트리가 있을 때에는 큰 사각형으로 우선체크한다.
			D3DXVECTOR3 pos[6];
			D3DXVECTOR3 min = box->GetOrgMin();
			D3DXVECTOR3 max = box->GetOrgMax();
			pos[0] = D3DXVECTOR3(min.x, 0.0f, min.z);
			pos[1] = D3DXVECTOR3(max.x, 0.0f, min.z);
			pos[2] = D3DXVECTOR3(min.x, 0.0f, max.z);
			pos[3] = D3DXVECTOR3(max.x, 0.0f, min.z);
			pos[4] = D3DXVECTOR3(min.x, 0.0f, max.z);
			pos[5] = D3DXVECTOR3(max.x, 0.0f, max.z);

			for (size_t i = 0; i < 6; i+=3)
			{
				if (D3DXIntersectTri(&pos[i], &pos[i+1], &pos[i+2],
									 &start, &direction, &u, &v, &distance) == TRUE)
				{
					bool bRes = false;
					for (size_t j = 0; j < 4; j++)
					{
						bRes |= Next[j]->MousePickked(start, direction, position);
					}

					if (bRes == false)
					{
						printf("");
					}

					return bRes;
				}
			}
		}
		else
		{
			map<float, D3DXVECTOR2> picks;

			for (UINT i = 0; i < IndexCount; i += 3)
			{
				float u, v, distance;
				if (D3DXIntersectTri(&VertexData[IndexData[i]].position,
									 &VertexData[IndexData[i + 1]].position,
									 &VertexData[IndexData[i + 2]].position,
									 &start, &direction, &u, &v, &distance) == TRUE)
				{
					float minx = VertexData[IndexData[i]].position.x;
					float minz = VertexData[IndexData[i]].position.z;

					minx = Math::Min(minx, VertexData[IndexData[i + 1]].position.x);
					minz = Math::Min(minz, VertexData[IndexData[i + 1]].position.z);
					minx = Math::Min(minx, VertexData[IndexData[i + 2]].position.x);
					minz = Math::Min(minz, VertexData[IndexData[i + 2]].position.z);

					picks[distance] = D3DXVECTOR2(minx, minz);
				}
			}

			if (picks.size() > 0)
			{
				position = (*picks.begin()).second;

				picks.clear();

				return true;
			}

			return false;
		}
	}
};

namespace Landscape
{
	class TerrainQuadTree
	{
	public:
		TerrainQuadTree(wstring file = L"", float heightRatio = 10.0f);
		~TerrainQuadTree();

		void SetHeightMapFile(wstring file, float heightRatio = 10.0f);
		void SetAlphaMapFile(UINT index, wstring file);

		void Render(class ExecuteValues* values);

		float GetHeight(D3DXVECTOR3 position);
		bool MousePickked(D3DXVECTOR3 start, D3DXVECTOR3 direction, OUT D3DXVECTOR2& position);

		void CopyVertexDataTo(OUT VertexTypePTNC2** data);
		void CopyVertexDataFrom(IN VertexTypePTNC2* data);

		void MoveVertexUpDown(D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ, int intensity);
		void MoveVertexUpDown(D3DXVECTOR2 positionXZ, float radius, int intensity);
		void MoveVertexSmooth(D3DXVECTOR2 positionXZ, float radius, int intensity);
		void AdjustVertexAlpha(UINT selectTexture, D3DXVECTOR2 positionXZ, float radius, int intensity);
		void AdjustVertexAlpha(UINT selectTexture, D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ, int intensity);

		void ChangeHeightRatio(float ratio);

		void SaveHeightMap(wstring filename);
		void SaveAlphaMap(UINT index, wstring filename);

		void ClearVertexHeight(void);
		void ClearVertexAlpha(void);

		void GetTerrainSize(UINT* width, UINT* height);

		void SetIsLanded(bool isLanded, D3DXVECTOR2 grid);

	private:
		void Clear(void);

		void FillNormalData(void);

		void CreateTree(QuadTree* tree, vector<D3DXCOLOR>& pixels);


		void UpdateVertexData(bool calcNormalData = true, D3D11_BOX* box = NULL);

	public:
		inline wstring GetHeightMapFile(void) { return heightMapFile; }
		inline wstring GetAlphaMapFile(UINT index) { assert(index < ALPHAMAP_MAX); return alphaMapFile[index]; }

	private:
		wstring heightMapFile;
		Texture* heightMap;

		wstring alphaMapFile[2];


		float heightRatio;

		UINT width, height;

		QuadTree* root;

	};
}