#pragma once

#define SPOTLIGHT_MAX 32

class SpotLight
{
public:
	friend LightManager;
private:
	SpotLight();
	~SpotLight();

	void Update(vector<pair<wstring, SpotLightData>>& lights);

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
			SpotLightData Light[SPOTLIGHT_MAX];
			
			int LightCount;
			float Padding[3];
		} Data;
	};

private:
	Buffer* buffer;
};