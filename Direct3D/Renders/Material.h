#pragma once

#include "../Interfaces/ICloneable.h"

class MaterialBuffer : public ShaderBuffer
{
public:
	MaterialBuffer()
		: ShaderBuffer(&Data, sizeof(Data))
	{
		Data.Ambient = D3DXCOLOR(0, 0, 0, 1);
		Data.Diffuse = D3DXCOLOR(1, 1, 1, 1);
		Data.Specular = D3DXCOLOR(1, 1, 1, 1);
		Data.Emissive = D3DXCOLOR(0, 0, 0, 1);

		Data.Shininess = 1;
		Data.DetailFactor = 1;
	}

	struct Struct
	{
		D3DXCOLOR Ambient;
		D3DXCOLOR Diffuse;
		D3DXCOLOR Specular;
		D3DXCOLOR Emissive;

		float Shininess;
		float DetailFactor;
		float Padding[2];
	} Data;
};

class Material : public ICloneable
{
public:
	Material();
	Material(wstring filename);
	~Material();

	// ICloneable을(를) 통해 상속됨
	virtual void Clone(void** clone) override;

public:
	void SetShader(string filename);
	void SetShader(wstring filename);
	void SetShader(Shader* shader);

	void SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetEmissiveMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetEmissiveMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetDetailMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDetailMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetBuffer(void);


public:
	inline wstring GetName(void)		{ return name; }
	inline void SetName(wstring val)	{ name = val; }
	inline Shader* GetShader(void)		{ return shader; }

	inline MaterialBuffer* GetBuffer(void) { return buffer; }


	inline D3DXCOLOR* GetAmbient(void)  { return &buffer->Data.Ambient; }
	inline D3DXCOLOR* GetDiffuse(void)  { return &buffer->Data.Diffuse; }
	inline D3DXCOLOR* GetSpecular(void) { return &buffer->Data.Specular; }
	inline D3DXCOLOR* GetEmissive(void) { return &buffer->Data.Emissive; }
	inline float* GetShininess(void)	{ return &buffer->Data.Shininess; }
	inline float* GetDetailFactor(void) { return &buffer->Data.DetailFactor; }


	inline void SetAmbient(D3DXCOLOR val)				{ SetAmbient(val.r, val.g, val.b); }
	inline void SetDiffuse(D3DXCOLOR val)				{ SetDiffuse(val.r, val.g, val.b); }
	inline void SetSpecular(D3DXCOLOR val)				{ SetSpecular(val.r, val.g, val.b); }
	inline void SetEmissive(D3DXCOLOR val)				{ SetEmissive(val.r, val.g, val.b); }
	inline void SetAmbient(float r, float g, float b)	{ buffer->Data.Ambient = D3DXCOLOR(r, g, b, 1); }
	inline void SetDiffuse(float r, float g, float b)	{ buffer->Data.Diffuse = D3DXCOLOR(r, g, b, 1); }
	inline void SetSpecular(float r, float g, float b)	{ buffer->Data.Specular = D3DXCOLOR(r, g, b, 1); }
	inline void SetEmissive(float r, float g, float b)	{ buffer->Data.Emissive = D3DXCOLOR(r, g, b, 1); }
	inline void SetShininess(float val)					{ buffer->Data.Shininess = val; }
	inline void SetDetailFactor(float val)				{ buffer->Data.DetailFactor = val; }


	inline Texture* GetDiffuseMap(void) { return diffuseMap; }
	inline Texture* GetSpecularMap(void) { return specularMap; }
	inline Texture* GetEmissiveMap(void) { return emissiveMap; }
	inline Texture* GetNormalMap(void) { return normalMap; }
	inline Texture* GetDetailMap() { return detailMap; }



private:
	wstring name;

	bool bShaderDelete;
	Shader* shader;

	MaterialBuffer* buffer;

	Texture* diffuseMap;
	Texture* specularMap;
	Texture* emissiveMap;
	Texture* normalMap;
	Texture* detailMap;

};