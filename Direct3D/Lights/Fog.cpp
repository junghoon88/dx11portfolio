#include "stdafx.h"
#include "Fog.h"

Fog::Fog()
{
	buffer = new Buffer();

	buffer->Data.Color = D3DXCOLOR(0.75f, 0.75f, 0.75f, 1);
	buffer->Data.Start = 0;
	buffer->Data.End = 100;
}

Fog::~Fog()
{
}

void Fog::Update()
{
}

void Fog::Render()
{
	ImGui::Separator();
	ImGui::SliderFloat3("Fog Color",	buffer->Data.Color, 0, 1);
	ImGui::SliderFloat("Fog Start",		&buffer->Data.Start, 0, 1000);
	ImGui::SliderFloat("Fog End",		&buffer->Data.End, 0, 1000);

	buffer->SetPSBuffer(12);
}
