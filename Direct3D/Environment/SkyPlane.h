#pragma once

namespace Environment
{
	class SkyPlane
	{
	private:
		struct SkyPlaneType
		{
			D3DXVECTOR3 pos;
			D3DXVECTOR2 uv;
		};

		class SkyPlaneBuffer : public ShaderBuffer
		{
		public:
			SkyPlaneBuffer() : ShaderBuffer(&Data, sizeof(Struct))
			{
				Data.translation = 0.0f;
				Data.scale = 0.3f;
				Data.brightness = 0.5f;
			}

			struct Struct
			{
				float translation;
				float scale;
				float brightness;
				float padding;
			} Data;
		};


	public:
		SkyPlane(wstring cloudFile, wstring perturbFile);
		~SkyPlane();

		void LoadData(Json::Value* value);
		void SaveData(Json::Value* value);
		void Update(void);
		void Render(void);
		void PostRender(void);


	private:
		void CreateSkyPlane(void);
		void ClearSkyPlane(void);

		void CreateStates(void);


	private:
		UINT vertexCount;
		UINT indexCount;
		ID3D11Buffer* vertexBuffer = NULL;
		ID3D11Buffer* indexBuffer = NULL;
		Texture* cloudTexture = NULL;
		Texture* perturbTexture = NULL;

		Shader* shader;
		SkyPlaneBuffer* skyPlaneBuffer;

		ID3D11RasterizerState* rasterizerState[2];
		ID3D11SamplerState* defaultSampleState;
		ID3D11BlendState* blendState[2];


		bool bShowWireFrame;
		// 하늘 평면 매개 변수
		int   skyPlaneResolution;
		float skyPlaneWidth;
		float skyPlaneTop;
		float skyPlaneBottom;
		int   textureRepeat;
	};
}