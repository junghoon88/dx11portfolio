#pragma once
#include "GameModel.h"

class MeshCapsule : public GameModel
{
public:
	MeshCapsule();
	~MeshCapsule();

	void Update(void) override;
	void Render(void) override;
	void PostRender(void) override;

private:
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	float shininess;
};