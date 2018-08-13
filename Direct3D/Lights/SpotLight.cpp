#include "stdafx.h"
#include "SpotLight.h"

SpotLight::SpotLight()
{
	buffer = new Buffer();
}

SpotLight::~SpotLight()
{
	SAFE_DELETE(buffer);
}

void SpotLight::Update(vector<pair<wstring, SpotLightData>>& lights)
{
	buffer->Data.LightCount = lights.size() < SPOTLIGHT_MAX ? lights.size() : SPOTLIGHT_MAX;

	for (int i = 0; i < buffer->Data.LightCount; i++)
	{
		buffer->Data.Light[i] = lights[i].second;
	}

	buffer->SetPSBuffer(11);
}

