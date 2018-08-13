#include "stdafx.h"
#include "PointLight.h"

PointLight::PointLight()
{
	buffer = new Buffer();
}

PointLight::~PointLight()
{
	SAFE_DELETE(buffer);
}

void PointLight::Update(vector<pair<wstring, PointLightData>>& lights)
{
	buffer->Data.LightCount = lights.size() < POINTLIGHT_MAX ? lights.size() : POINTLIGHT_MAX;

	for (int i = 0; i < buffer->Data.LightCount; i++)
	{
		buffer->Data.Light[i] = lights[i].second;
	}

	buffer->SetPSBuffer(10);
}

