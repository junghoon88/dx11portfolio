#include "stdafx.h"
#include "Tree.h"
#include "../Objects/MeshQuad.h"
#include "../Objects/GameModel.h"


Landscape::Tree::Tree(ExecuteValues* values, bool bFixedY)
	: values(values), bFixedY(bFixedY)
{
	for (int i = 0; i < 4; i++)
	{
		quad[i] = new MeshQuad();
		quad[i]->SetShader(Shaders + L"Tree.hlsl");
		quad[i]->SetDiffuse(Textures + L"Tree.png");
		//하위 transform 의 Y축 각도를 45도씩 회전
		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, Math::ToRadian(45.0f * i));
		quad[i]->SetRootAxis(R);
	}

	worldBuffer = new WorldBuffer();

	buffer = new Buffer();
}

Landscape::Tree::~Tree()
{
	SAFE_DELETE(buffer);
	SAFE_DELETE(worldBuffer);

	for (int i = 0; i < 4; i++)
		SAFE_DELETE(quad[i]);
}

void Landscape::Tree::Update(void)
{
	//if (values->editableFlag & (ULONG)ExcuteFlags::Tree)
	//{
	//}

	for (int i = 0; i < 4; i++)
		quad[i]->Update();

	buffer->Data.FixedY = bFixedY == false ? 0 : 1;
	buffer->Data.Position = transform.Position;
	buffer->Data.Scale = transform.Scale;
}

void Landscape::Tree::Render(void)
{
	if (values->renderFlag & (ULONG)ExcuteFlags::Tree)
	{
		worldBuffer->SetVSBuffer(1);

		buffer->SetVSBuffer(2);

		for (UINT i = 0; i < 4; i++)
		{
			quad[i]->Render();
		}
	}
}
