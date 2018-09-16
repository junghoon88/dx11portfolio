#pragma once

class GameUnit;

class UnitHpBar
{
public:
	UnitHpBar(GameUnit* unit);
	~UnitHpBar();

	void Update(float hpRatio);
	void Render(void);

private:
	GameUnit* unit;

	Shader* shader;

	VertexTexture*  vertexData;
	UINT*			indexData;
	ID3D11Buffer*	vertexBuffer;
	ID3D11Buffer*	indexBuffer;

	Texture*		texture[2];

	
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.width	 = 100.0f;
			Data.height  = 20.0f;
			Data.yOffset = 22.0f;
			Data.hpRatio = 1.0f;

			Data.colorHP = D3DXCOLOR(0.8f, 0, 0, 1);
			Data.colorBack = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1);
		}
		struct Struct
		{
			float width;
			float height;
			float yOffset;
			float hpRatio;

			D3DXCOLOR colorHP;
			D3DXCOLOR colorBack;
		} Data;
	};

	Buffer* buffer;
};
