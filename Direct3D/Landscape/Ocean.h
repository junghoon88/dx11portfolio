#pragma once

namespace Landscape
{
	class Ocean
	{
	public:
		Ocean(ExecuteValues* values, UINT width, UINT height, UINT thick, 
				UINT xOffset, UINT yOffset);
		~Ocean();

		void Update(void);
		void Render(void);
		void PostRender(void);

		void CreateVertex(void);

		void LoadData(Json::Value* value);
		void SaveData(Json::Value* value);


	public:
		inline void SetHeightMap(Texture* map) { heightMap = map; }

	private:
		ExecuteValues* values;

		Material* material;
		WorldBuffer* worldBuffer;


		ID3D11Buffer* vertexBuffer, *indexBuffer;
		OceanVertexType* vertices;
		UINT* indices;
		UINT vertexCount, indexCount;

		UINT width, height;
		UINT thick;
		UINT xOffset, yOffset;

		bool bWireframe;
		ID3D11RasterizerState* rasterizer[2];
		ID3D11BlendState* blendState[2];

		Texture* heightMap;


	private:
		class VSBuffer : public ShaderBuffer
		{
		public:
			VSBuffer() : ShaderBuffer(&Data, sizeof(Data))
			{
				Data.WaveFrequency = 0.05f;
				Data.WaveAmplitude = 1.0f;
				Data.TextureScale  = D3DXVECTOR2(14, 14);
				Data.BumpSpeed     = D3DXVECTOR2(0.0f, 0.05f);
				Data.BumpHeight    = 0.5f;
			}
			struct Struct
			{
				float WaveFrequency;
				float WaveAmplitude;
				D3DXVECTOR2 TextureScale;
				D3DXVECTOR2 BumpSpeed;
				float BumpHeight;
				float Padding;
			} Data;
		};

		class PSBuffer : public ShaderBuffer
		{
		public:
			PSBuffer() : ShaderBuffer(&Data, sizeof(Data))
			{
				Data.ShallowColor = D3DXCOLOR(0.7f, 0.85f, 0.8f, 1.0f);
				Data.DeepColor = D3DXCOLOR(0.2f, 0.5f, 0.95f, 1.0f);

				Data.FresnelBias = 0.8f;
				Data.FresnelPower = 0.5f;
				Data.FresnelAmount = 0.5f;
				Data.SpecularAmount = 0.5f;

				Data.HeightRatio = 7.5;
				Data.ShoreBlend = 35.0f;
			}
			struct Struct
			{
				D3DXCOLOR ShallowColor; //������
				D3DXCOLOR DeepColor;    //������

				//������ ������
				float FresnelBias;      //����
				float FresnelPower;     //����
				float FresnelAmount;    //������

				float SpecularAmount;   //�ݻ簭��

				D3DXVECTOR2 OceanSize;  //���ؽ��� ��üũ��
				float  HeightRatio;     //���� ������
				float  ShoreBlend;      //���İ� ��ȭ����
			} Data;
		};

		VSBuffer* vsBuffer;
		PSBuffer* psBuffer;
	};
}