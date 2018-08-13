#pragma once

#define TEXTURE_MAX		6

namespace Landscape
{
	class TerrainData;

	class GameTerrain
	{
	public:
		GameTerrain(ExecuteValues* values);
		~GameTerrain();

		void Update(void) {}
		void Render(void);
		void PostRender(void) {}

		float GetVertexHeight(D3DXVECTOR3 position);

	private:
		void LoadJsonTerrain(wstring filename);

	private:
		ExecuteValues*		values;

		TerrainData*		terrainData;

		Material*			material;
		WorldBuffer*		worldBuffer;

		ID3D11SamplerState* diffuseSampler;
		ID3D11SamplerState* detailSampler;

		//alpha ¸Ê ÅØ½ºÃÄ
		Texture*				textures[TEXTURE_MAX];

		//Tree
		//vector<Landscape::Tree*> trees;

		//Astar test
		//AStar* astar;
	};

}