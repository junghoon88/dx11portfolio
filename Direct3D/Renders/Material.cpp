#include "stdafx.h"
#include "Material.h"
#include "../Utilities/String.h"

Material::Material()
	: bShaderDelete(false), shader(NULL)
	, diffuseMap(NULL), specularMap(NULL), emissiveMap(NULL), normalMap(NULL), detailMap(NULL)
{
	buffer = new MaterialBuffer();
}

Material::Material(wstring filename)
	: bShaderDelete(false), shader(NULL)
	, diffuseMap(NULL), specularMap(NULL), emissiveMap(NULL), normalMap(NULL), detailMap(NULL)
{
	if (filename.length() > 0)
	{
		bShaderDelete = true;
		shader = new Shader(filename);
	}

	buffer = new MaterialBuffer();
}


Material::~Material()
{
	if (bShaderDelete == true)
		SAFE_DELETE(shader);

	SAFE_DELETE(diffuseMap);
	SAFE_DELETE(specularMap);
	SAFE_DELETE(emissiveMap);
	SAFE_DELETE(normalMap);
	SAFE_DELETE(detailMap);

	SAFE_DELETE(buffer);
}

void Material::Clone(void** clone)
{
	Material* temp = NULL;

	//TODO : Shader 내부 생성인경우 예외처리 필요
	if (this->shader)
	{
		temp = new Material(this->shader->GetFile());
	}
	else
	{
		temp = new Material();
	}

	temp->SetName(this->name);
	//temp->shader; //생성자에서
	//temp->buffer; //생성자에서
	
	temp->SetAmbient(this->buffer->Data.Ambient);
	temp->SetDiffuse(this->buffer->Data.Diffuse);
	temp->SetSpecular(this->buffer->Data.Specular);
	temp->SetEmissive(this->buffer->Data.Emissive);
	temp->SetShininess(this->buffer->Data.Shininess);
	temp->SetDetailFactor(this->buffer->Data.DetailFactor);


	if(this->diffuseMap)  temp->SetDiffuseMap(this->diffuseMap->GetFile());
	if(this->specularMap) temp->SetSpecularMap(this->specularMap->GetFile());
	if(this->emissiveMap) temp->SetEmissiveMap(this->emissiveMap->GetFile());
	if(this->normalMap)   temp->SetNormalMap(this->normalMap->GetFile());
	if(this->detailMap)   temp->SetDetailMap(this->detailMap->GetFile());

	*clone = temp;
}


void Material::SetShader(string file)
{
	SetShader(String::ToWString(file));
}

void Material::SetShader(wstring filename)
{
	if (filename.length() == 0)
		return;

	if (bShaderDelete)
	{
		SAFE_DELETE(shader);
	}

	bShaderDelete = true;
	shader = new Shader(filename);
}

void Material::SetShader(Shader * shader)
{
	if (bShaderDelete)
	{
		SAFE_DELETE(this->shader);
		bShaderDelete = false;
	}

	this->shader = shader;
}


void Material::SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDiffuseMap(String::ToWString(file));
}

void Material::SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if (file.length() == 0)
	{
		SAFE_DELETE(diffuseMap);
		return;
	}

	Texture* temp = new Texture(file, info);
	SAFE_DELETE(diffuseMap);
	diffuseMap = temp;
}

void Material::SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetSpecularMap(String::ToWString(file));
}

void Material::SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if (file.length() == 0)
	{
		SAFE_DELETE(specularMap);
		return;
	}

	Texture* temp = new Texture(file, info);
	SAFE_DELETE(specularMap);
	specularMap = temp;
}

void Material::SetEmissiveMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetEmissiveMap(String::ToWString(file));
}

void Material::SetEmissiveMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if (file.length() == 0)
	{
		SAFE_DELETE(emissiveMap);
		return;
	}

	Texture* temp = new Texture(file, info);
	SAFE_DELETE(emissiveMap);
	emissiveMap = temp;
}

void Material::SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetNormalMap(String::ToWString(file));
}

void Material::SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if (file.length() == 0)
	{
		SAFE_DELETE(normalMap);
		return;
	}

	Texture* temp = new Texture(file, info);
	SAFE_DELETE(normalMap);
	normalMap = temp;
}

void Material::SetDetailMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDetailMap(String::ToWString(file));
}

void Material::SetDetailMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if (file.length() == 0)
	{
		SAFE_DELETE(detailMap);
		return;
	}

	Texture* temp = new Texture(file, info);
	SAFE_DELETE(detailMap);
	detailMap = temp;
}

void Material::SetBuffer(void)
{
	UINT slot = 0;

	if (diffuseMap != NULL)
	{
		diffuseMap->SetShaderResource(slot);
		diffuseMap->SetShaderSampler(slot);
	}
	else
	{
		//Texture::SetBlankShaderResource(slot);
		//Texture::SetBlankSamplerState(slot);
	}


	slot = 1;
	if (specularMap != NULL)
	{
		specularMap->SetShaderResource(slot);
		specularMap->SetShaderSampler(slot);
	}
	else
	{
		//Texture::SetBlankShaderResource(slot);
		//Texture::SetBlankSamplerState(slot);
	}


	slot = 2;
	if (emissiveMap != NULL)
	{
		//emissiveMap->SetShaderResource(slot);
		//emissiveMap->SetShaderSampler(slot);
	}
	else
	{
		//Texture::SetBlankShaderResource(slot);
		//Texture::SetBlankSamplerState(slot);
	}


	slot = 3;
	if (normalMap != NULL)
	{
		normalMap->SetShaderResource(slot);
		normalMap->SetShaderSampler(slot);
	}
	else
	{
		//Texture::SetBlankShaderResource(slot);
		//Texture::SetBlankSamplerState(slot);
	}


	slot = 4;
	if (detailMap != NULL)
	{
		detailMap->SetShaderResource(slot);
		detailMap->SetShaderSampler(slot);
	}
	else
	{
		//Texture::SetBlankShaderResource(slot);
		//Texture::SetBlankSamplerState(slot);
	}

	buffer->SetPSBuffer(1);

	if(shader)	shader->Render();
}

