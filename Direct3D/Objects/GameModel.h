#pragma once
#include "GameRender.h"

class GameModel : public GameRender
{
private:
	class BoneBuffer;
	class RenderBuffer;

public:
	GameModel(wstring file);
	virtual ~GameModel();

public:
	virtual void Update(ModelShape* modelShape);
	virtual void Render(void);
	virtual void PostRender(void) {}


	D3DXVECTOR3 CalcVelocity(D3DXVECTOR3& vel);
	void AddPosition(D3DXVECTOR3& vec);
	void SetPosition(D3DXVECTOR3& vec);

	void GetVelocity(D3DXVECTOR3& vec);
	void SetVelocity(D3DXVECTOR3& vec);

	void Rotate(D3DXVECTOR2 amount);

protected:
	void CalcPosition(void);

public:
	inline Model* GetModel(void) { return model; }
	inline Shader* GetShader(void) { return shader; }
	inline wstring GetFilePath(void) { return Path::GetDirectoryName(matFile); }
	inline wstring GetFileName(void) { return Path::GetFileNameWithoutExtension(matFile); }
	inline void GetFileInfo(wstring& mat, wstring& mesh) { mat = matFile; mesh = meshFile; }

protected:
	wstring matFile;
	wstring meshFile;

	Shader* shader;
	Model* model;

	D3DXVECTOR3 velocity;
	D3DXMATRIX rotateMatrix;

	vector<D3DXMATRIX> boneTransforms;

	//buffer
	BoneBuffer*		boneBuffer;
	RenderBuffer*	renderBuffer;


	//Buffer class 정의
private:
	class BoneBuffer : public ShaderBuffer
	{
	public:
		BoneBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 128; i++)
				D3DXMatrixIdentity(&Data.Bones[i]);
		}

		void SetBones(D3DXMATRIX* m, UINT count)
		{
			if (count > 128)
				count = 128;

			memcpy(Data.Bones, m, sizeof(D3DXMATRIX) * count);

			//shader 전달하기 위해 transpose
			for (UINT i = 0; i < count; i++)
				D3DXMatrixTranspose(&Data.Bones[i], &Data.Bones[i]);
		}

	private:
		struct Struct
		{
			D3DXMATRIX Bones[128];
		} Data;
	};

	class RenderBuffer : public ShaderBuffer
	{
	public:
		RenderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.BoneNumber = 0;
		}

	public:
		struct Struct
		{
			int BoneNumber;

			float Padding[3];
		} Data;
	};

};
