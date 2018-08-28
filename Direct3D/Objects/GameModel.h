#pragma once
#include "GameRender.h"
#include "AnimationClip.h"
#include "AnimationBlender.h"

struct AnimationBlender;
enum class AnimationPlayMode;
class Bounding;

enum class ANIMATION_TYPE
{
	Unknown = -1, Mixamo, 
};

class GameModel : public GameRender
{
private:
	class BoneBuffer;
	class RenderBuffer;

	friend class EditModel;

public:
	GameModel(wstring file, ANIMATION_TYPE animType = ANIMATION_TYPE::Unknown);
	virtual ~GameModel();

public:
	virtual void Update(void);
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

	UINT AddClipAll(wstring path);
	UINT AddClip(wstring file, bool bLoop = false);
	UINT AddClip(AnimationClip* clip);
	void RemoveClip(UINT index);
	void ClearClips(void);

	AnimationClip* GetClip(UINT index);
	AnimationBlender* GetBlenderFromBoneName(wstring name);

	bool PlayAnim(UINT index);
	bool PlayAnim(UINT index, float startTime, float blendTime, float timeScaleFactor);
	void PauseAnim(bool pause);
	void StopAnim(void);
	float GetPlayTime(wstring boneName);
	float GetPlayProgress(void);


public:
	inline void GetFileInfo(wstring& mat, wstring& mesh) { mat = matFile; mesh = meshFile; }
	inline ANIMATION_TYPE GetAnimType(void) { return animType; }

	inline Model* GetModel(void) { return model; }
	inline Shader* GetShader(void) { return shader; }
	inline wstring GetFilePath(void) { return Path::GetDirectoryName(matFile); }
	inline wstring GetFileName(void) { return Path::GetFileNameWithoutExtension(matFile); }

	inline vector<AnimationBlender*>& GetBlendersRef(void) { return blenders; }
	inline vector<AnimationClip*>& GetClipsRef(void) { return clips; }

	inline vector<Bounding*>& GetBoundingsRef(void) { return boundings; }

protected:
	wstring matFile;
	wstring meshFile;

	ANIMATION_TYPE animType;

	Shader* shader;
	Model* model;

	D3DXVECTOR3 velocity;
	D3DXMATRIX rotateMatrix;

	vector<D3DXMATRIX> boneTransforms;

	//animation
	vector<AnimationClip*> clips;
	vector<AnimationBlender*> blenders;
	bool bPause;

	//capsule
	vector<Bounding*> boundings;


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
