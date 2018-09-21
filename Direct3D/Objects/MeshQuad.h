#pragma once
#include "GameModel.h"

class MeshQuad : public GameModel
{
public:
	MeshQuad();
	~MeshQuad();

	void Update(void) override;
	void Render(void) override;
	void PostRender(void) override;

	void SetShader(wstring file);
	void SetDiffuse(D3DXCOLOR color);
	void SetDiffuse(wstring file);
	void SetSpecular(D3DXCOLOR color);
	void SetSpecular(wstring file);

};