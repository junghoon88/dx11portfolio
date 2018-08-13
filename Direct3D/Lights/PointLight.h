#pragma once

#define POINTLIGHT_MAX 32

class PointLight
{
public:
	friend LightManager;

private:
	PointLight();
	~PointLight();

	void Update(vector<pair<wstring, PointLightData>>& lights);

private:

	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.LightCount = 0;
		}

		struct Struct
		{
			PointLightData Light[POINTLIGHT_MAX];

			int LightCount;
			float Padding[3];
		} Data;
	};

	Buffer* buffer;
};
