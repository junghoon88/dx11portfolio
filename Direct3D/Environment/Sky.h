#pragma once

namespace Environment
{
	class SkyPlane;

	class Sky
	{
	public:
		class SkyBuffer : public ShaderBuffer
		{
		public:
			SkyBuffer() : ShaderBuffer(&Data, sizeof(Struct))
			{
				Data.HorizonColorTwilight	= D3DXCOLOR( 0.4f, 0.16f,  0.0f, 1.0f) * 1.5f;
				Data.HorizonColorDay		= D3DXCOLOR( 1.0f,  1.0f,  1.0f, 1.0f);
				Data.HorizonColorNight		= D3DXCOLOR( 0.2f,  0.2f,  0.2f, 1.0f);
				Data.CeilingColorTwilight	= D3DXCOLOR(0.17f, 0.15f, 0.15f, 1.0f);
				Data.CeilingColorDay		= D3DXCOLOR(0.72f, 0.75f, 0.98f, 1.0f);
				Data.CeilingColorNight		= D3DXCOLOR( 0.1f,  0.1f, 0.15f, 1.0f);

				Data.Height = 0.4f;
				Data.Time = 0.0f;
			}

			struct Struct
			{
				D3DXCOLOR HorizonColorTwilight;	// 황혼 수직 컬러
				D3DXCOLOR HorizonColorDay;		// 주간 수직 컬러
				D3DXCOLOR HorizonColorNight;	// 야간 수직 컬러

				D3DXCOLOR CeilingColorTwilight;	// 황혼 윗면
				D3DXCOLOR CeilingColorDay;		// 주간 윗면
				D3DXCOLOR CeilingColorNight;	// 야간 윗면

				float Height;
				float Time;
				float Padding[2];
			} Data;
		};

		Sky(ExecuteValues* values);
		~Sky();

		void LoadData(Json::Value* value);
		void SaveData(Json::Value* value);
		void Update(void);
		void Render(void);
		void PostRender(void);

	private:
		ExecuteValues*		values;

		Model*				sphere;
		SkyBuffer*			skyBuffer;

		wstring				shaderFile;
		wstring				modelFile;



		ID3D11RasterizerState*		rasterizer[2];
		ID3D11DepthStencilState*	depthStencilState[2];

		D3DXMATRIX world;


		//외부 클래스 변수
		SkyPlane* skyPlane;
	};
}
