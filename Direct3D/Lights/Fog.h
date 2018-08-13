#pragma once

class Fog
{
public:
	Fog();
	~Fog();

	void Update();
	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(0, 0, 0, 1);
			Data.Start = 0;
			Data.End = 0;
		}

		struct Struct
		{
			D3DXCOLOR Color;
			float  Start;
			float  End;
			float Padding[2];
		} Data;
	};

private:
	Buffer* buffer;
};