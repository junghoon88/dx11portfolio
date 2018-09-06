#include "stdafx.h"
#include "Model.h"
#include "ModelAnimClip.h"

Model::Model()
	: root(NULL)
{
}

Model::~Model()
{
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
	UINT boneSize = bones.size();
	transforms.assign(boneSize, D3DXMATRIX());

	for (UINT i = 0; i < boneSize; i++)
	{
		ModelBone* bone = bones[i];

		//부모 노드의 좌표를 더한다
		if (bone->parent == NULL)
		{
			transforms[i] = bone->transform * matrix;
		}
		else
		{
			transforms[i] = bone->transform * transforms[bone->parent->index];
		}
	}
}

D3DXMATRIX Model::GetAbsoluteBoneTo(UINT index)
{
	assert(index < bones.size());

	D3DXMATRIX mat = bones[index]->GetTransform();
	if (bones[index]->GetParent())
	{
		return mat * GetAbsoluteBoneTo(bones[index]->GetParentIndex());
	}

	return mat;
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
	if (bone == NULL)
		bone = root;

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

//bool Model::MousePickked(D3DXVECTOR3 start, D3DXVECTOR3 direction, OUT float & dist)
//{
//	size_t size = meshes.size();
//
//	vector<float> distances;
//	for (size_t i = 0; i < size; i++)
//	{
//		float temp;
//		if (meshes[i]->MousePickked(start, direction, temp))
//		{
//			distances.push_back(temp);
//		}
//	}
//	
//	if (distances.size() > 0)
//	{
//
//		return true;
//	}
//
//	vector<D3DXMATRIX> transforms;
//	CopyAbsoluteBoneTo(transforms);
//
//	for (ModelMesh* mesh : meshes)
//	{
//		int index = mesh->GetParentBoneIndex();
//		D3DXMATRIX transform = transforms[index];
//
//		mesh->SetWorld(transform);
//		
//		float temp;
//		if (mesh->MousePickked(start, direction, temp))
//		{
//			distances.push_back(temp);
//		}
//	}
//
//	
//	return false;
//}

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

