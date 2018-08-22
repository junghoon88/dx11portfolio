#pragma once

class Grid3Axis
{
public:
	Grid3Axis(int gridSize = 50);
	~Grid3Axis();

	void Render(void);
private:
	void CreateVertex(void);

private:
	Shader* shader;

	WorldBuffer* worldBuffer;

	int gridSize;
	UINT vertexCount, indexCount;
	ID3D11Buffer* vertexBuffer, *indexBuffer;

	VertexColor* vertexData;
	UINT* indexData;
};
