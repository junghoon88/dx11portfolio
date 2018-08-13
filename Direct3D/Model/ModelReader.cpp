#include "stdafx.h"
#include "Model.h"

#include "ModelMeshPart.h"
#include "ModelAnimClip.h"
#include "ModelAnimPlayer.h"
#include "../Utilities/BinaryFile.h"

void Model::ReadMaterial(wstring file)
{
	Models::LoadMaterial(file, &materials);
}

void Model::ReadMesh(wstring file)
{
	Models::LoadMesh(file, &bones, &meshes);

	BindMeshData();
}

void Model::ReadAnimation(wstring file)
{
	Models::LoadAnimation(file, &clips);
}


void Model::BindMeshData(void)
{
	//Material 연결
	for (ModelMesh* mesh : meshes)
	{
		for (ModelMeshPart* part : mesh->meshParts)
		{
			part->material = GetMaterial(part->materialName);
		}
	}

	if (bones.size() > 0)
	{
		this->root = bones[0];
		for (ModelBone* bone : bones)
		{
			if (bone->parentIndex > -1)
			{
				bone->parent = bones[bone->parentIndex];
				bone->parent->childs.push_back(bone);
			}
			else
			{
				bone->parent = NULL;
			}
		}
	}

	for (ModelMesh* mesh : meshes)
	{
		mesh->parentBone = bones[mesh->parentBoneIndex];
		mesh->Binding();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
map<wstring, vector<Material*>> Models::materialMap;
void Models::LoadMaterial(wstring file, vector<Material*>* materials)
{
	if (materialMap.count(file) < 1)
	{
		ReadMaterialData(file);
	}

	for(Material* material : materialMap[file])
	{
		Material* temp = NULL;
		material->Clone((void**)&temp);

		materials->push_back(temp);
	}
}

void Models::ReadMaterialData(wstring file)
{
	vector<Material*> srcMaterial;

	Json::Value root;
	Json::ReadData(file, &root);


	Json::Value::iterator iter = root.begin();
	for (; iter != root.end(); iter++)
	{
		Json::Value value = (*iter);
		Material* material = new Material();

		string name;
		Json::GetValue(value, "Name", name);
		material->SetName(String::ToWString(name));

		Json::GetValue(value, "ShaderName", name);
		if (name.length() > 0)
			material->SetShader(Shaders + String::ToWString(name));


		D3DXCOLOR color;
		Json::GetValue(value, "Ambient", color);
		material->SetAmbient(color);

		Json::GetValue(value, "Diffuse", color);
		material->SetDiffuse(color);

		Json::GetValue(value, "Emissive", color);
		material->SetEmissive(color);

		Json::GetValue(value, "Specular", color);
		material->SetSpecular(color);


		float shininess;
		Json::GetValue(value, "Shininess", shininess);
		material->SetShininess(shininess);


		string textureFile;
		string directory = Path::GetDirectoryName(String::ToString(file));

		Json::GetValue(value, "DiffuseFile", textureFile);
		if (textureFile.length() > 0)
			material->SetDiffuseMap(directory + textureFile);

		Json::GetValue(value, "SpecularFile", textureFile);
		if (textureFile.length() > 0)
			material->SetSpecularMap(directory + textureFile);

		Json::GetValue(value, "EmissiveFile", textureFile);
		if (textureFile.length() > 0)
			material->SetEmissiveMap(directory + textureFile);

		Json::GetValue(value, "NormalFile", textureFile);
		if (textureFile.length() > 0)
			material->SetNormalMap(directory + textureFile);

		srcMaterial.push_back(material);
	}

	materialMap[file] = srcMaterial;
}

map<wstring, Models::MeshData> Models::meshDataMap;
void Models::LoadMesh(wstring file, vector<ModelBone*>* bones, vector<ModelMesh*>* meshes)
{
	if (meshDataMap.count(file) < 1)
	{
		ReadMeshData(file);
	}

	MeshData data = meshDataMap[file];
	for (ModelBone* bone : data.Bones)
	{
		ModelBone* temp = NULL;
		bone->Clone((void**)&temp);

		bones->push_back(temp);
	}
	for (ModelMesh* mesh : data.Meshes)
	{
		ModelMesh* temp = NULL;
		mesh->Clone((void**)&temp);

		meshes->push_back(temp);
	}
}

void Models::ReadMeshData(wstring file)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);


	vector<ModelBone*> bones;
	vector<ModelMesh*> meshes;

	//////////////////////Bone 데이터//////////////////////////
	UINT boneCount = r->UInt();
	for (UINT i = 0; i < boneCount; i++)
	{
		ModelBone* bone = new ModelBone();
		bone->index = r->Int();
		bone->name = String::ToWString(r->String());
		bone->parentIndex = r->Int();
		bone->transform = r->Matrix();
		bone->absTransform = r->Matrix();

		bones.push_back(bone);
	}
	//////////////////////////////////////////////////////////

	UINT meshCount = r->UInt();
	for (UINT i = 0; i < meshCount; i++)
	{
		ModelMesh* mesh = new ModelMesh();
		mesh->name = String::ToWString(r->String());

		int parentBoneIndex = r->Int();
		//번호와 본을 검색해 부모를 찾는다.
		for (ModelBone* bone : bones)
		{
			if (parentBoneIndex == bone->index)
			{
				mesh->parentBoneIndex = parentBoneIndex;
				mesh->parentBone = bone;
				break;
			}
		}//for(bone : bones)

		UINT partCount = r->UInt();
		for (UINT j = 0; j < partCount; j++)
		{
			ModelMeshPart* meshPart = new ModelMeshPart();
			meshPart->parent = mesh;

			meshPart->materialName = String::ToWString(r->String());

			//VertexData
			{
				UINT count = r->UInt();
				meshPart->vertices.assign(count, ModelVertexType());

				void* ptr = (void*)&(meshPart->vertices[0]);
				r->Byte(&ptr, sizeof(ModelVertexType) * count);
			}

			//IndexData
			{
				UINT count = r->UInt();
				meshPart->indices.assign(count, UINT());

				void* ptr = (void*)&(meshPart->indices[0]);
				r->Byte(&ptr, sizeof(UINT) * count);
			}

			mesh->meshParts.push_back(meshPart);
		}
		meshes.push_back(mesh);
	}

	r->Close();
	SAFE_DELETE(r);

	MeshData data;
	data.Bones.assign(bones.begin(), bones.end());
	data.Meshes.assign(meshes.begin(), meshes.end());
	meshDataMap[file] = data;
}

map<wstring, vector<ModelAnimClip*>> Models::animClipMap;
void Models::LoadAnimation(wstring file, vector<ModelAnimClip*>* clips)
{
	if (animClipMap.count(file) == 0)
		ReadAnimationData(file);

	vector<ModelAnimClip*> data = animClipMap[file];
	for (ModelAnimClip* clip : data)
	{
		ModelAnimClip* temp = NULL;
		clip->Clone((void**)&temp);
		clips->push_back(temp);
	}
}

void Models::ReadAnimationData(wstring file)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = r->UInt();
	vector<ModelAnimClip *> clips;
	for (UINT i = 0; i < count; i++)
	{
		ModelAnimClip* clip = new ModelAnimClip();
		clip->filename = Path::GetFileNameWithoutExtension(file);
		clip->name = String::ToWString(r->String());

		clip->totalFrame = r->UInt();
		clip->frameRate = r->Float();
		clip->defaltFrameRate = clip->frameRate;

		UINT frameCount = r->UInt();
		for (UINT frame = 0; frame < frameCount; frame++)
		{
			ModelKeyframe* keyframe = new ModelKeyframe();
			keyframe->BoneName = String::ToWString(r->String());


			UINT count = r->UInt();
			keyframe->Datas.assign(count, ModelKeyframeData());

			void* ptr = (void *)&(keyframe->Datas[0]);
			r->Byte(&ptr, sizeof(ModelKeyframeData) * count);

			clip->keyframeMap[keyframe->BoneName] = keyframe;
		}//for(frame)

		clips.push_back(clip);
	}

	r->Close();
	SAFE_DELETE(r);

	animClipMap[file] = clips;
}
