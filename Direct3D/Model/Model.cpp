#include "stdafx.h"
#include "Model.h"
#include "ModelAnimClip.h"

Model::Model()
	: root(NULL)
{
	buffer = new ModelBuffer();
}

Model::~Model()
{
	SAFE_DELETE(buffer);

	for (Material* mat : materials)
		SAFE_DELETE(mat);

	for (ModelBone* bone : bones)
		SAFE_DELETE(bone);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);

	for (ModelAnimClip* clip : clips)
		SAFE_DELETE(clip);
}

void Model::Render(void)
{
	vector<D3DXMATRIX> transforms;
	CopyAbsoluteBoneTo(transforms);

	for (ModelMesh* mesh : meshes)
	{
		int index = mesh->GetParentBoneIndex();
		D3DXMATRIX transform = transforms[index];

		mesh->SetWorld(transform);
		mesh->Render();
	}
}

Material* Model::GetMaterial(wstring name)
{
	for (Material* material : materials)
	{
		if (material->GetName() == name)
			return material;
	}
	return NULL;
}

ModelMesh* Model::GetMesh(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->GetName() == name)
			return mesh;
	}
	return NULL;
}

ModelBone * Model::GetBone(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->GetName() == name)
			return bone;
	}
	return NULL;
}

void Model::CopyAbsoluteBoneTo(vector<D3DXMATRIX>& transforms)
{
	transforms.clear();
	transforms.assign(bones.size(), D3DXMATRIX());

	for (UINT i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];

		//부모 노드의 좌표를 더한다
		if (bone->parent != NULL)
		{
			int index = bone->parent->index;
			transforms[i] = bone->transform * transforms[index];
		}
		else
			transforms[i] = bone->transform;
	}
}

void Model::CopyAbsoluteBoneTo(D3DXMATRIX matrix, vector<D3DXMATRIX>& transforms)
{
	transforms.clear();
	transforms.assign(bones.size(), D3DXMATRIX());

	for (UINT i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];

		//부모 노드의 좌표를 더한다
		if (bone->parent == NULL)
		{
			transforms[i] = bone->transform * matrix;
		}
		else
		{
			int index = bone->parent->index;
			transforms[i] = bone->transform * transforms[index];
		}
	}
}

void Model::ScanPointMinMax(D3DXVECTOR3 * min, D3DXVECTOR3 * max)
{
	for (ModelMesh* mesh : meshes)
	{
		mesh->ScanPointMinMax(min, max);
	}
}

void Model::ShowTreeNode(ModelBone* bone)
{
	string boneName = String::ToString(bone->GetName());
	char num[10];
	itoa(bone->GetIndex(), num, 10);
	boneName += string(" - ") + string(num);
	if (ImGui::TreeNode(boneName.c_str()))
	{
		UINT childnum = bone->GetChildCount();
		for (UINT i = 0; i < childnum; i++)
		{
			ShowTreeNode(bone->GetChild(i));
		}
		ImGui::TreePop();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Models::Create(void)
{
}

void Models::Delete(void)
{
	for (pair<wstring, vector<Material *>> temp : materialMap)
	{
		for (Material* material : temp.second)
			SAFE_DELETE(material);
	}

	for (pair<wstring, MeshData> temp : meshDataMap)
	{
		MeshData data = temp.second;

		for (ModelBone* bone : data.Bones)
			SAFE_DELETE(bone);

		for (ModelMesh* mesh : data.Meshes)
			SAFE_DELETE(mesh);
	}
}

