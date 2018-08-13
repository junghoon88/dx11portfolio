#pragma once

class Model;
class ModelAnimClip;
class ModelBone;

class ModelAnimPlayer
{
public:
	enum class Mode
	{
		Play = 0, Pause, Stop
	};

	ModelAnimPlayer(Model* model);
	~ModelAnimPlayer();

	void Update(D3DXMATRIX& matWorld);
	void Render(void);

	void SelectClip(UINT index, bool useSlerp = true);
	void RefreshClip(void);
	bool isCurrentClip(UINT index);
	void SetMode(Mode mode);

public:
	inline ModelAnimClip* GetCurrentClip(void) { return currentClip; }
	inline int GetCurrentKeyframe(void) { return currentKeyframe; }

	inline bool GetEndAnim(void) { return bEndAnim; }

	inline D3DXMATRIX GetBoneAnimation(UINT index) { return boneAnimation[index]; }
	inline D3DXMATRIX GetSkinTransform(UINT index) { return skinTransform[index]; }

private:
	void UpdateTime(void);
	void UpdateBone(void);

private:
	Shader* shader;

	Model* model;
	ModelAnimClip* currentClip;

	Mode mode;

	int currentKeyframe;
	float frameTime;		//현재 프레임부터 경과시간?
	float keyframeFactor;	//현재 프레임과 다음 프레임 사이의 보간값

	int nextKeyframe;

	bool bUseSlerp;

	bool bEndAnim;

	vector<D3DXMATRIX> boneAnimation;
	vector<D3DXMATRIX> skinTransform;
};
