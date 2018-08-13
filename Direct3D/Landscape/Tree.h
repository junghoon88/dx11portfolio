#pragma once

namespace Model { class BinModel; }

namespace Landscape
{
	class Tree
	{
	private:
		class Buffer : public ShaderBuffer
		{
		public:
			Buffer() : ShaderBuffer(&Data, sizeof(Struct))
			{
				Data.FixedY = 0;
				Data.Position = { 0, 0, 0 };
				Data.Scale = { 1, 1, 1 };
			}

			struct Struct
			{
				UINT		FixedY;
				D3DXVECTOR3 Position;

				D3DXVECTOR3 Scale;
				float		Padding;
			} Data;
		};
	public:
		Tree(ExecuteValues* values, bool bFixedY = false);
		~Tree();

		void Update(void);
		void Render(void);


	public:
		//TRANSFROM
		inline void SetTransform(TRANSFORM tr)				{ transform = tr; }
		inline void SetTransformScale(D3DXVECTOR3 vec)		{ transform.Scale = vec; }
		inline void SetTransformRotateDeg(D3DXVECTOR3 vec)	{ transform.RotationDeg = vec; }
		inline void SetTransformPosition(D3DXVECTOR3 vec)	{ transform.Position = vec; }
		inline void SetTransformTranslate(D3DXVECTOR3 vec)	{ transform.Position += vec; }
		inline TRANSFORM GetTransform(void)					{ return transform; }
		inline D3DXVECTOR3 GetTransformScale(void)			{ return transform.Scale; }
		inline D3DXVECTOR3 GetTransformRotateDeg(void)		{ return transform.RotationDeg; }
		inline D3DXVECTOR3 GetTransformPosition(void)		{ return transform.Position; }
		inline D3DXVECTOR3& GetTransformScaleRef(void)		{ return transform.Scale; }
		inline D3DXVECTOR3& GetTransformRotateDegRef(void)	{ return transform.RotationDeg; }
		inline D3DXVECTOR3& GetTransformPositionRef(void)	{ return transform.Position; }

		inline void SetFiexedY(bool val) { bFixedY = val; }


	private:
		ExecuteValues*		values;

		Model::BinModel*	model;
		WorldBuffer*		worldBuffer;

		Buffer*				buffer;

		TRANSFORM			transform;

		bool bFixedY;
	};
}