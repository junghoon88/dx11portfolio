#pragma once

class MeshQuad;

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
		inline void SetFiexedY(bool val) { bFixedY = val; }
		inline TRANSFORM* GetTransform(void) { return &transform; }

	private:
		ExecuteValues*		values;

		MeshQuad*			quad[4];
		WorldBuffer*		worldBuffer;

		Buffer*				buffer;

		TRANSFORM			transform;

		bool				bFixedY;
	};
}