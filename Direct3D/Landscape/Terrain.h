#pragma once

#include "../GamePlayManager/EnemyRespawnManager.h"

#define TEXTURE_MAX		6

#define ENEMYAREA_MAX	10	//수정시 shader 파일도 수정해야함


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
		EDITMODE_ALPHATEXTURE,
		EDITMODE_HEIGHTUPDOWN,
		EDITMODE_HEIGHTSMOOTH,
		EDITMODE_MAKETREE,
		EDITMODE_ENEMYAREA,
		EDITMODE_ASTARTEST,

		EDITMODE_MAX
	};

	class TerrainData;
	class TerrainQuadTree;
	class Tree;


	class Terrain
	{
	private:
		class GridBuffer;
		class EnemyAreaBuffer;

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

		void PostRenderHeightMap(void);
		void PostRenderTextures(void);
		void PostRenderEnemyArea(void);

		//void AddTrees(D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ, int intensity);
		//void AddTrees(D3DXVECTOR2 positionXZ, float radius, int intensity);
		//void DeleteTrees(D3DXVECTOR2 startXZ, D3DXVECTOR2 sizeXZ);
		//void DeleteTrees(D3DXVECTOR2 positionXZ, float radius);

		void CreateTotalDiffuse(wstring filename);
		void CreateTotalNormal(wstring filename);

	public:
		void ChangeShader(wstring filename);

		void ChangeVertexData(TerrainVertexType* vertexData);

		void ChangeDiffuseMap(wstring filename);
		void ChangeNormalMap(wstring filename);
		void ChangeSubDiffuse(UINT index, wstring filename);
		void ChangeSubNormal(UINT index, wstring filename);
		D3DXVECTOR2 GetHoveringGrid(void);


	public:
		//inline TerrainData* GetTerrainData(void) { return terrainData; }
		inline UINT GetGridSpacing(void) { assert(gridBuffer != NULL); return gridBuffer->Data.GridSpacing; }
		//inline AStar* GetAStar(void) { return astar; }
		inline vector<EnemyArea>& GetEnemyAreasRef(void) { return enemyAreas; }

		inline TerrainData* GetTerrainData(void) { return terrainData; }

	private:
		ExecuteValues*		values;

		TerrainData*		terrainData;
		//TerrainQuadTree*		terrainData;

		Material*			material;
		WorldBuffer*		worldBuffer;
		GridBuffer*			gridBuffer;
		EnemyAreaBuffer*    enemyAreaBuffer;

		ID3D11SamplerState* diffuseSampler;
		ID3D11SamplerState* detailSampler;


	private:
		eEditMode				editmode;
		int						intensity;

		//terrain 정보 백업용
		D3DXVECTOR2				clickedGrid;
		bool					bChangeTerrain;
		TerrainVertexType*		vertexDataOld;

		//alpha 맵 텍스쳐
		UINT					selectTexture;
		Texture*				subDiffuse[TEXTURE_MAX];
		Texture*				subNormal[TEXTURE_MAX];

		//Tree
		//vector<Landscape::Tree*> trees;

		//Astar test
		//AStar* astar;

		vector<EnemyArea>		enemyAreas;
		int						showAreaIndex;


	private:
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

				Data.HoverGridStart = D3DXVECTOR2(0, 0);
				Data.HoverGridSize = D3DXVECTOR2(0, 0);
				Data.SelectGridStart = D3DXVECTOR2(0, 0);
				Data.SelectGridSize = D3DXVECTOR2(0, 0);

				Data.ColorGrid = D3DXCOLOR(1, 1, 1, 1);

				Data.ColorHover = D3DXCOLOR(1, 0, 0, 1);
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

		class EnemyAreaBuffer : public ShaderBuffer
		{
		public:
			EnemyAreaBuffer() : ShaderBuffer(&Data, sizeof(Struct))
			{
				for (size_t i = 0; i < ENEMYAREA_MAX; i++)
				{
					Data.area[i].Start = D3DXVECTOR2(0, 0);
					Data.area[i].Size  = D3DXVECTOR2(0, 0);
					Data.area[i].Color = D3DXCOLOR(1, 1, 1, 1);
				}
			}

			void SetArea(size_t index, EnemyArea& area)
			{
				Data.area[index].Start = area.Start;
				Data.area[index].Size  = area.Size;
				Data.area[index].Color = area.Color;
			}

		private:
			struct Area
			{
				D3DXVECTOR2 Start;
				D3DXVECTOR2 Size;
				D3DXCOLOR   Color;
			};

			struct Struct
			{
				Area area[ENEMYAREA_MAX];
			} Data;
		};
	};

}