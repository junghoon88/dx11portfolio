#pragma once
#include "GameModel.h"
#include "AnimationClip.h"

struct AnimationBlender;
enum class AnimationPlayMode;

class GameAnimationModel : public GameModel
{
public:
	GameAnimationModel(wstring file);
	virtual ~GameAnimationModel();

	virtual void Update(ModelShape* modelShape);
	virtual void Render(void);
	virtual void PostRender(void);


	UINT AddClipAll(wstring path);
	UINT AddClip(wstring file);
	UINT AddClip(AnimationClip* clip);
	void RemoveClip(UINT index);
	void ClearClips(void);

	AnimationClip* GetClip(UINT index);
	AnimationBlender* GetBlenderFromBoneName(wstring name);

	bool PlayAnim(UINT index, AnimationPlayMode mode);
	bool PlayAnim(UINT index, float startTime, float blendTime, float timeScaleFactor, AnimationPlayMode mode);
	float GetPlayTime(wstring boneName);

public:
	inline vector<AnimationBlender*>& GetBlendersRef(void) { return blenders; }
	inline vector<AnimationClip*>& GetClipsRef(void) { return clips; }


private:
	vector<AnimationClip*> clips;

	vector<AnimationBlender*> blenders;
};