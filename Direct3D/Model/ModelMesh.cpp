#include "stdafx.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"

ModelMesh::ModelMesh()
	: parentBoneIndex(0), parentBone(NULL)
{
	worldBuffer = new WorldBuffer();
}

ModelMesh::~ModelMesh()
{
	SAFE_DELETE(worldBuffer);

	for (ModelMeshPart* part : meshParts)
	{
		SAFE_DELETE(part);
	}
}

void ModelMesh::Clone(void** clone)
{
	ModelMesh* mesh = new ModelMesh();

	mesh->name = this->name;

	mesh->parentBoneIndex = this->parentBoneIndex;

	//ModelBone* parentBone;

	//WorldBuffer* worldBuffer;
	//vector<ModelMeshPart*> meshParts;
	for (ModelMeshPart* part : meshParts)
	{
		ModelMeshPart* temp = NULL;
		part->Clone((void**)&temp);

		temp->parent = mesh;
		mesh->meshParts.push_back(temp);
	}


	*clone = mesh;
}

void ModelMesh::Binding(void)
{
	for (ModelMeshPart* part : meshParts)
	{
		part->Binding();
	}
}

void ModelMesh::Render(void)
{
	worldBuffer->SetVSBuffer(1);

	for (ModelMeshPart* part : meshParts)
	{
		part->Render();
	}
}

void ModelMesh::ScanPointMinMax(D3DXVECTOR3 * min, D3DXVECTOR3 * max)
{
	for (ModelMeshPart* part : meshParts)
	{
		part->ScanPointMinMax(min, max, worldBuffer->GetWorld());
	}
}

