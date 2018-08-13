#pragma once

#include "AStar.h"

class AStar;

class AStarViewer
{
public:
	AStarViewer();
	~AStarViewer();

	void CreateBuffer(vTile tiles, D3DXCOLOR color = D3DXCOLOR(1, 1, 0, 1));

	void Render(void);

private:
	vector<VertexColor> vertexData;
	vector<UINT>		indexData;

	ID3D11Buffer*		vertexBuffer;
	ID3D11Buffer*		indexBuffer;

	WorldBuffer*		worldBuffer;

	Shader*				shader;

};

