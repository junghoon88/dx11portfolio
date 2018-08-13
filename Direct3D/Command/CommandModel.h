#pragma once
#include "stdafx.h"
#include "Command.h"

#include "../Renders/Material.h"
#include "../Model/Component.h"

//select
//load
//delete
//move(transform, boxcollider)

#if 0
class CommandModelLoad : public Command
{
public:
	CommandModelLoad(vector<Model*>& models, Model* addModel)
		: models(models), loaded(false)
	{
		loadModels.push_back(addModel);
	}
	CommandModelLoad(vector<Model*>& models, vector<Model*> addModels)
		: models(models), loadModels(addModels), loaded(false)
	{
		
	}
	~CommandModelLoad()
	{
		if (loaded == false)
		{
			for (Model* model : loadModels)
				SAFE_DELETE(model);
		}
	}

	// Command을(를) 통해 상속됨
	virtual void Execute(void) override
	{
		for (Model* model : loadModels)
		{
			models.push_back(model);
		}
		loaded = true;
	}

	virtual void Undo(void) override
	{
		assert(loadModels.back() == models.back());

		for (int i = (int)loadModels.size() - 1; i >= 0; --i)
		{
			assert(models.back() == loadModels[i]);
			models.pop_back();
		}
		loaded = false;
	}

	virtual void Redo(void) override
	{
		for (Model* model : loadModels)
		{
			models.push_back(model);
		}
		loaded = true;
	}

private:
	vector<Model*>& models;
	vector<Model*> loadModels;
	bool loaded;
};

class CommandModelDelete : public Command
{
public:
	CommandModelDelete(vector<Model*>& models, Model* deleteModel)
		: models(models), deleted(false)
	{
		deleteModels.push_back(deleteModel);
	}
	CommandModelDelete(vector<Model*>& models, vector<Model*> deleteModels)
		: models(models), deleteModels(deleteModels), deleted(false)
	{
	}

	~CommandModelDelete()
	{
		if (deleted == true)
		{
			for(Model* model : deleteModels)
				SAFE_DELETE(model);
		}
	}

	// Command을(를) 통해 상속됨
	virtual void Execute(void) override
	{
#if 1
		UINT count = deleteModels.size();
		for (UINT i = 0; i < deleteModels.size(); i++)
		{
			for (UINT j = 0; j < models.size(); j++)
			{
				if (models[j] == deleteModels[i])
				{
					//indexs[j] = i;
					models.erase(models.begin() + j);
					count--;
					break;
				}
			}
			if (count == 0) break;
		}
#else
		UINT count = deleteModels.size();
		for (UINT i = 0; i < deleteModels.size(); i++)
		{
			for (UINT j = 0; j < models.size(); j++)
			{
				if (models[j] == deleteModels[i])
				{
					//indexs[j] = i;
					models.erase(models.begin() + j);
					count--;
					break;
				}
			}
			if (count == 0) break;
		}

#endif

		assert(count == 0);
		deleted = true;
	}
	
	virtual void Undo(void) override
	{
		//models.resize(models.size() + 1);
		//for (UINT i = models.size()-1; i > index; i--)
		//{
		//	models[i] = models[i - 1];
		//}
		//models[index] = deleteModel;

		for (Model* model : deleteModels)
			models.push_back(model);

		deleted = false;
	}

	virtual void Redo(void) override
	{
		//models.erase(models.begin() + index);
		//deleted = true;

		Execute();
	}

private:
	vector<Model*>& models;
	vector<Model*>  deleteModels;
	bool deleted;
	//vector<int> indexs;
};

class CommandModelCopy : public Command
{
public:
	CommandModelCopy(vector<Model*>& models, vector<Model*> targetModels)
		: models(models), copied(false)
	{
		UINT size = targetModels.size();
		copyModels.resize(size);

		for (UINT i = 0; i < size; i++)
		{
			targetModels[i]->Clone((void**)&copyModels[i]);
		}
	}
	~CommandModelCopy()
	{
		if (copied == false)
		{
			for (Model* model : copyModels)
				SAFE_DELETE(model);
		}
	}
	
	// Command을(를) 통해 상속됨
	virtual void Execute(void) override
	{
		for (UINT i = 0; i < copyModels.size(); i++)
		{
			models.push_back(copyModels[i]);
		}
		copied = true;
	}

	virtual void Undo(void) override
	{
		assert(copyModels.back() == models.back());

		for (int i = (int)copyModels.size() - 1; i >= 0; --i)
		{
			assert(models.back() == copyModels[i]);
			models.pop_back();
		}
		copied = false;
	}

	virtual void Redo(void) override
	{
		Execute();
	}

private:
	vector<Model*>& models;
	vector<Model*>  copyModels;
	bool copied;
};

class CommandModelTransform : public Command
{
public:
	//모델들을 한곳으로 이동할 때
	CommandModelTransform(Model* model, TRANSFORM transform)
	{
		models.push_back(model);
		oldTransforms.resize(1);
		newTransforms.resize(1);

		oldTransforms[0] = models[0]->GetTransform();
		newTransforms[0] = oldTransforms[0];
		newTransforms[0] = transform;
	}
	//모델들을 이전 위치들을 받고 현재위치로 이동 했다는 커맨드 
	CommandModelTransform(vector<Model*> models, vector<TRANSFORM> oldTransforms)
		: models(models), oldTransforms(oldTransforms)
	{
		assert(models.size() > 0);
		assert(models.size() == oldTransforms.size());

		newTransforms.resize(models.size());

		for (UINT i = 0; i < models.size(); i++)
		{
			newTransforms[i] = this->models[i]->GetTransform();
		}
	}

	~CommandModelTransform()
	{
	}

	// Command을(를) 통해 상속됨
	virtual void Execute(void) override 
	{
		for(UINT i = 0; i < models.size(); i++)
			models[i]->SetTransform(newTransforms[i]);
	}

	virtual void Undo(void) override
	{
		for (UINT i = 0; i < models.size(); i++)
			models[i]->SetTransform(oldTransforms[i]);
	}

	virtual void Redo(void) override
	{
		Execute();
	}


private:
	vector<Model*> models;
	vector<TRANSFORM>		 oldTransforms;
	vector<TRANSFORM>		 newTransforms;

};

class CommandMeshTransform : public Command
{
public:
	//모델들을 이전 위치들을 받고 현재위치로 이동 했다는 커맨드 
	CommandMeshTransform(Model::MeshPart* part, TRANSFORM oldTransform, TRANSFORM newTransform)
		: part(part), oldTransform(oldTransform), newTransform(newTransform)
	{
		assert(part != NULL);
	}

	~CommandMeshTransform()
	{
	}

	// Command을(를) 통해 상속됨
	virtual void Execute(void) override
	{
		part->SetTransform(newTransform);
	}

	virtual void Undo(void) override
	{
		part->SetTransform(oldTransform);
	}

	virtual void Redo(void) override
	{
		Execute();
	}


private:
	Model::MeshPart*	part;
	TRANSFORM			oldTransform;
	TRANSFORM			newTransform;

};

enum class MaterialColorType
{
	BufferColor = 0,
	Ambient,
	Diffuse,
	Specular,
	Emissive,
	Shininess,
	DetailFactor,

};

class CommandMeshColor : public Command
{
public:
	CommandMeshColor(Model::MeshPart* part, MaterialColorType type, D3DXCOLOR oldColor, D3DXCOLOR color)
		: part(part), type(type), oldColor(oldColor), newColor(color)
	{
		assert(part != NULL);
	}
	CommandMeshColor(Model::MeshPart* part, MaterialColorType type, float oldValue, float newValue)
		: part(part), type(type), oldValue(oldValue), newValue(newValue)
	{
		assert(part != NULL);
	}
	~CommandMeshColor()
	{
	}

	// Command을(를) 통해 상속됨
	virtual void Execute(void) override
	{
		switch (type)
		{
		case MaterialColorType::BufferColor:	part->SetColor(newColor);						break;
		case MaterialColorType::Ambient:		part->GetMaterial()->SetAmbient(newColor);		break;
		case MaterialColorType::Diffuse:		part->GetMaterial()->SetDiffuse(newColor);		break;
		case MaterialColorType::Specular:		part->GetMaterial()->SetSpecular(newColor);		break;
		case MaterialColorType::Emissive:		part->GetMaterial()->SetEmissive(newColor);		break;
		case MaterialColorType::Shininess:		part->GetMaterial()->SetShininess(newValue);	break;
		case MaterialColorType::DetailFactor:	part->GetMaterial()->SetDetailFactor(newValue);	break;
		}
	}

	virtual void Undo(void) override
	{
		switch (type)
		{
		case MaterialColorType::BufferColor:	part->SetColor(oldColor);						break;
		case MaterialColorType::Ambient:		part->GetMaterial()->SetAmbient(oldColor);		break;
		case MaterialColorType::Diffuse:		part->GetMaterial()->SetDiffuse(oldColor);		break;
		case MaterialColorType::Specular:		part->GetMaterial()->SetSpecular(oldColor);		break;
		case MaterialColorType::Emissive:		part->GetMaterial()->SetEmissive(oldColor);		break;
		case MaterialColorType::Shininess:		part->GetMaterial()->SetShininess(oldValue);	break;
		case MaterialColorType::DetailFactor:	part->GetMaterial()->SetDetailFactor(oldValue);	break;
		}
	}

	virtual void Redo(void) override
	{
		Execute();
	}


private:
	Model::MeshPart*	part;
	MaterialColorType	type;
	D3DXCOLOR			oldColor;
	D3DXCOLOR			newColor;
	float				oldValue;
	float				newValue;
};

enum class MaterialFileType
{
	DiffuseMap = 0,
	SpecularMap,
	EmissiveMap,
	NormalMap,
	Shader
};

class CommandMaterialOpenFile : public Command
{
public:
	CommandMaterialOpenFile(Material* material, MaterialFileType type, wstring	oldFile, wstring newFile)
		: material(material), type(type), oldFile(oldFile), newFile(newFile)
	{

	}
	~CommandMaterialOpenFile()
	{

	}

	// Command을(를) 통해 상속됨
	virtual void Execute(void) override
	{
		if (newFile.size() == 0)
			return;

		switch (type)
		{
		case MaterialFileType::DiffuseMap:
			material->SetDiffuseMap(newFile);
			break;
		case MaterialFileType::SpecularMap:
			material->SetSpecularMap(newFile);
			break;
		case MaterialFileType::EmissiveMap:
			material->SetEmissiveMap(newFile);
			break;
		case MaterialFileType::NormalMap:
			material->SetNormalMap(newFile);
			break;
		case MaterialFileType::Shader:
			material->SetShader(newFile);
			break;
		default:
			assert(false);
			break;
		}
	}

	virtual void Undo(void) override
	{
		if (oldFile.size() == 0)
			return;

		switch (type)
		{
		case MaterialFileType::DiffuseMap:
			material->SetDiffuseMap(oldFile);
			break;
		case MaterialFileType::SpecularMap:
			material->SetSpecularMap(oldFile);
			break;
		case MaterialFileType::EmissiveMap:
			material->SetEmissiveMap(oldFile);
			break;
		case MaterialFileType::NormalMap:
			material->SetNormalMap(oldFile);
			break;
		case MaterialFileType::Shader:
			material->SetShader(oldFile);
			break;
		default:
			assert(false);
			break;
		}
	}

	virtual void Redo(void) override
	{
		Execute();
	}



private:
	Material*	material;
	wstring		oldFile;
	wstring		newFile;
	MaterialFileType	type;
};
#endif