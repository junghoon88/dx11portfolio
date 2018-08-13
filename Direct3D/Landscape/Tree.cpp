#include "stdafx.h"
#include "Tree.h"

#include "../Utilities/Math.h"

Landscape::Tree::Tree(ExecuteValues* values, bool bFixedY)
	: values(values), bFixedY(bFixedY)
{
	Fbx::FbxModels::ReadBinary(BinModels + L"Meshes/Quad.model", &model);
	worldBuffer = new WorldBuffer();

	Material* material = model->GetMeshPart(0)->GetMaterial();
	material->SetShader(Shaders + L"Tree.hlsl");
	material->SetDiffuseMap(Textures + L"Tree.png");

	if(model->GetMeshPartSize() < 4)
		model->CopyParts(0, 3);

	for (UINT i = 0; i < 4; i++)
	{
		//하위 transform 의 Y축 각도를 45도씩 회전
		model->GetMeshPart(i)->SetTransformRotateDeg(D3DXVECTOR3(0.0f, 45.0f * i, 0.0f));
	}

	buffer = new Buffer();
}

Landscape::Tree::~Tree()
{
	SAFE_DELETE(buffer);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(model);
}

void Landscape::Tree::Update(void)
{
	if (values->editableFlag & (ULONG)ExcuteFlags::Tree)
	{
	}

	for (UINT i = 0; i < 4; i++)
	{
		model->GetMeshPart(i)->Update();
	}

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
			model->GetMeshPart(i)->Render();
		}
	}
}
