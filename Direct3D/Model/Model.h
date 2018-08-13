#pragma once

class ModelMesh;
class ModelBone;
class ModelAnimClip;

class ModelBuffer : public ShaderBuffer
{
public:
	ModelBuffer() : ShaderBuffer(&data, sizeof(data))
	{
		for (int i = 0; i < 128; i++)
			D3DXMatrixIdentity(&data.Bones[i]);
	}

	void SetBones(D3DXMATRIX* m, UINT count)
	{
		assert(count <= 128);

		memcpy(data.Bones, m, sizeof(D3DXMATRIX) * count);

		for (UINT i = 0; i < count; i++)
			D3DXMatrixTranspose(&data.Bones[i], &data.Bones[i]);
	}

private:
	struct Struct
	{
		D3DXMATRIX Bones[128];
	} data;
};

class Model
{
public:
	Model();
	~Model();

	void Render(void);

	//ModelReader.cpp
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadAnimation(wstring file);
private:
	void BindMeshData(void);
	//~ModelReader.cpp

public:
	vector<Material*>& GetMaterials(void) { return materials; }

	Material* GetMaterial(wstring name);
	ModelMesh* GetMesh(wstring name);
	ModelBone* GetBone(wstring name);

	//Àý´ë ÁÂÇ¥
	void CopyAbsoluteBoneTo(vector<D3DXMATRIX>& transforms);
	void CopyAbsoluteBoneTo(D3DXMATRIX matrix, vector<D3DXMATRIX>& transforms);


	void ScanPointMinMax(D3DXVECTOR3* min, D3DXVECTOR3* max);
	void ShowTreeNode(ModelBone* bone);


public:
	inline ModelBone* GetRoot(void) { return root; }
	inline ModelBone* GetBone(UINT index) { return bones[index]; }
	inline ModelMesh* GetMesh(UINT index) { return meshes[index]; }
	inline ModelAnimClip* GetClip(UINT index)
	{
		if (index < clips.size())
			return clips[index];
		return NULL;
	}
	inline UINT GetClipCount(void) { return clips.size(); }

	inline ModelBuffer* Getbuffer(void) { return buffer; };

	inline vector<ModelMesh*>& GetMeshesRef(void) { return meshes; }

	inline UINT GetMeshCount(void) { return meshes.size(); }
	inline UINT GetBoneCount(void) { return bones.size(); }


private:
	ModelBone*				root;

	vector<Material*>		materials;
	vector<ModelBone*>		bones;
	vector<ModelMesh*>		meshes;
	vector<ModelAnimClip*>	clips;


	ModelBuffer* buffer;
};

class Models
{
public:
	friend Model;

public:
	static void Create(void);
	static void Delete(void);

private:
	//ModelReader.cpp
	static void LoadMaterial(wstring file, vector<Material*>* materials);
	static void ReadMaterialData(wstring file);

	static void LoadMesh(wstring file, vector<ModelBone*>* bones, vector<ModelMesh*>* meshes);
	static void ReadMeshData(wstring file);

	static void LoadAnimation(wstring file, vector<ModelAnimClip*>* clips);
	static void ReadAnimationData(wstring file);

public:
	static map<wstring, vector<Material*>> materialMap;

	struct MeshData
	{
		vector<ModelBone*> Bones;
		vector<ModelMesh*> Meshes;
	};
	static map<wstring, MeshData> meshDataMap;

	static map<wstring, vector<ModelAnimClip*>> animClipMap;
};