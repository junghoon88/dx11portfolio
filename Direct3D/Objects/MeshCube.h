#pragma once
#include "GameModel.h"

class MeshCube : public GameModel
{
public:
	MeshCube();
	~MeshCube();

	void Update(ModelShape* modelShape) override;
	void Render(void) override;
	void PostRender(void) override;

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.ClipPlane = D3DXVECTOR4(0, -1, 0, 1);
		}

		struct Struct
		{
			D3DXVECTOR4 ClipPlane;
		} Data;
	};


private:
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	float shininess;

	Buffer* buffer;
};