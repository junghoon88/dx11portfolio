#pragma once

#include "../Model/Bounding/BoundingBox.h"

#define ALPHAMAP_MAX	2


struct QuadTree
{
	int				Level;
	UINT			WidthOffset;
	UINT			HeightOffset;
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
		SAFE_RELEASE(IndexBuffer);
		SAFE_DELETE_ARRAY(IndexData);
		for (size_t i = 0; i < 4; i++)
			SAFE_DELETE(Next[i]);
	}
	void Render(FrustumCulling* frustumCulling, VertexTypePTNC2* vertexData)
	{
		bool bNext = false;
		for (int i = 0; i < 4; i++)
		{
			if (Next[i] != NULL)
			{
				bNext = true;
				Next[i]->Render(frustumCulling, vertexData);
			}
		}

		if (bNext == false)
		{
			//bool bContain = false;
			//for (UINT i = 0; i < IndexCount; i++)
			//{
			//	if (frustumCulling->ContainPoint(vertexData[IndexData[i]].position))
			//	{
			//		bContain = true;
			//		break;
			//	}
			//}
			//
			//if (bContain)
			{
				gDC->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
				gDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				gDC->DrawIndexed(IndexCount, 0, 0);
			}
		}

		box->Update();
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

		void Render(FrustumCulling* frustumCulling);

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

		void FillVertexData(float heightRatio);
		void FillNormalData(void);

		void CreateBuffer(void);
		void CreateTree(QuadTree* tree);


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
		UINT vertexCount;
		ID3D11Buffer* vertexBuffer;

		QuadTree* root;

		VertexTypePTNC2* vertexData;
	};
}