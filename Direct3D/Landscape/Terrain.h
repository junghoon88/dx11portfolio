#pragma once

#define TEXTURE_MAX		6

class AStar;

namespace Landscape
{
	enum eBrushType
	{
		BRUSHTYPE_NONE = 0,
		BRUSHTYPE_ROUNDBRUSH,
		BRUSHTYPE_GRID,

		BRUSHTYPE_MAX
	};

	enum eEditMode
	{
		EDITMODE_HEIGHTUPDOWN,
		EDITMODE_HEIGHTSMOOTH,
		EDITMODE_ALPHATEXTURE,
		EDITMODE_MAKETREE,
		EDITMODE_ASTARTEST,

		EDITMODE_MAX
	};

	class TerrainData;
	class Tree;

	class GridBuffer : public ShaderBuffer
	{
	public:
		GridBuffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.bView = 0;
			Data.GridSpacing = 1;
			Data.GridThickness = 0.03f;

			Data.BrushType = BRUSHTYPE_NONE; //enum eBrushType
			Data.Radius = 5.0f;
			Data.Position = D3DXVECTOR2(0, 0);

			Data.HoverGridStart  = D3DXVECTOR2(0, 0);
			Data.HoverGridSize   = D3DXVECTOR2(0, 0);
			Data.SelectGridStart = D3DXVECTOR2(0, 0);
			Data.SelectGridSize  = D3DXVECTOR2(0, 0);

			Data.ColorGrid     = D3DXCOLOR(1, 1, 1, 1);

			Data.ColorHover    = D3DXCOLOR(1, 0, 0, 1);
			Data.ColorSelected = D3DXCOLOR(0, 0, 1, 1);
		}

		struct Struct
		{
			UINT		bView;
			UINT		GridSpacing;
			float		GridThickness;
			float		Padding;

			//brush
			UINT		BrushType; //enum eBrushType
			float		Radius;
			D3DXVECTOR2 Position;

			//grid
			D3DXVECTOR2 HoverGridStart;
			D3DXVECTOR2 HoverGridSize;
			D3DXVECTOR2 SelectGridStart;
			D3DXVECTOR2 SelectGridSize;


			D3DXCOLOR	ColorGrid;

			D3DXCOLOR	ColorHover;
			D3DXCOLOR	ColorSelected;
		} Data;
	};

	class Terrain
	{
	public:
		Terrain(ExecuteValues* values);
		~Terrain();

		void LoadData(Json::Value* value);
		void SaveData(Json::Value* value);
		void Update(void);
		void Render(void);
		void PostRender(void);

		float GetVertexHeight(D3DXVECTOR3 position);

	private:
		void MousePick(void);
		void UpdateMouse(void);
		void UpdateKeyboard(void);
		void PostRenderEditMenu(void);

		//void AddTrees(D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ, int intensity);
		//void AddTrees(D3DXVECTOR2 positionXZ, float radius, int intensity);
		//void DeleteTrees(D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ);
		//void DeleteTrees(D3DXVECTOR2 positionXZ, float radius);

		void SaveJsonTerrain(wstring filename);
		void LoadJsonTerrain(wstring filename);
		void SaveXmlTerrain(wstring filename);
		void LoadXmlTerrain(wstring filename);

	public:
		void ChangeVertexData(VertexTypePTNC2* vertexData);

		void ChangeDiffuseMap(wstring filename);
		void ChangeTexture(UINT index, wstring filename);
		D3DXVECTOR2 GetHoveringGrid(void);


	public:
		inline TerrainData* GetTerrainData(void) { return terrainData; }
		inline UINT GetGridSpacing(void) { assert(gridBuffer != NULL); return gridBuffer->Data.GridSpacing; }
		//inline AStar* GetAStar(void) { return astar; }


	private:
		ExecuteValues*		values;

		TerrainData*		terrainData;

		Material*			material;
		WorldBuffer*		worldBuffer;
		GridBuffer*			gridBuffer;

		ID3D11SamplerState* diffuseSampler;
		ID3D11SamplerState* detailSampler;


	private:
		eEditMode				editmode;
		int						intensity;

		//terrain 정보 백업용
		D3DXVECTOR2				clickedGrid;
		bool					bChangeTerrain;
		VertexTypePTNC2*		vertexDataOld;

		//alpha 맵 텍스쳐
		UINT					selectTexture;
		Texture*				textures[TEXTURE_MAX];

		//Tree
		//vector<Landscape::Tree*> trees;

		//Astar test
		//AStar* astar;
	};

}