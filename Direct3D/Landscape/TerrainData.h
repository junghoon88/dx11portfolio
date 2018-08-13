#pragma once


#define ALPHAMAP_MAX	2


enum class SellAttribute
{
	NONE,
	MOVE_DISABLE,

};

struct Sell
{
	SellAttribute Attribute = SellAttribute::NONE;
	float AvgHeight = 0.0f;
	//D3DXVECTOR3 Normal = D3DXVECTOR3(0, 0, 0);
	bool isLanded = false;
};

namespace Landscape
{
	class TerrainData
	{
	public:
		TerrainData(wstring file = L"", float heightRatio = 10.0f);
		~TerrainData();

		void SetHeightMapFile(wstring file, float heightRatio = 10.0f);
		void SetAlphaMapFile(UINT index, wstring file);

		void Render(void);

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

		void SetAttribute(SellAttribute attribute, D3DXVECTOR2 gridStart, D3DXVECTOR2 gridSize);
		void SetAttribute(SellAttribute attribute, D3DXVECTOR2 grid);
		void SetIsLanded(bool isLanded, D3DXVECTOR2 grid);

	private:
		void Clear(void);

		void FillVertexData(float heightRatio);
		void FillNormalData(void);
		void FillSellData(void);

		void CreateBuffer(void);

		void UpdateVertexData(bool calcNormalData = true, D3D11_BOX* box = NULL);
		void UpdateAstarSell(void);

	public:
		inline wstring GetHeightMapFile(void) { return heightMapFile; }
		inline wstring GetAlphaMapFile(UINT index) { assert(index < ALPHAMAP_MAX); return alphaMapFile[index]; }
		inline Sell* GetAstarSells(void) { return AstarSells; }

	private:
		wstring heightMapFile;
		Texture* heightMap;

		wstring alphaMapFile[2];


		float heightRatio;

		UINT width, height;
		UINT vertexCount, indexCount;
		ID3D11Buffer* vertexBuffer, *indexBuffer;

		VertexTypePTNC2* vertexData;
		UINT* indexData;
		Sell* AstarSells;

	};
}