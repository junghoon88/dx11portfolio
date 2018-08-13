#include "stdafx.h"
#include "GameAnimationModel.h"

#include "AnimationBlender.h"

GameAnimationModel::GameAnimationModel(wstring file)
	: GameModel(file)
{
	for (UINT i = 0; i < model->GetBoneCount(); i++)
	{
		ModelBone* bone = model->GetBone(i);

		AnimationBlender* blender = new AnimationBlender();
		blender->Name = bone->GetName();

		blenders.push_back(blender);
	}
}

GameAnimationModel::~GameAnimationModel()
{
	ClearClips();

	for (AnimationBlender* blender : blenders)
		SAFE_DELETE(blender);
}

void GameAnimationModel::Update(ModelShape* modelShape)
{
	if (clips.size() == 0)
	{
		GameModel::Update(modelShape);
		return;
	}

	
	CalcPosition();

	UINT boneCount = model->GetBoneCount();
	for (UINT i = 0; i < boneCount; i++)
	{
		AnimationBlender* blender = blenders[i];

		if (blender->Exist())
		{
			float time = Time::Delta();
			D3DXMATRIX mat = blender->GetKeyframeMatrix(time);
			model->GetBone(i)->SetTransform(mat);
		}
	}

	D3DXMATRIX transformed = Transformed();
	model->CopyAbsoluteBoneTo(transformed, boneTransforms);
}

void GameAnimationModel::Render(void)
{
	GameModel::Render();
}

void GameAnimationModel::PostRender(void)
{

}

UINT GameAnimationModel::AddClipAll(wstring path)
{
	vector<wstring> files;
	Path::GetFiles(&files, path, L"*.anim", false);
	for (size_t i = 0; i < files.size(); i++)
	{
		AddClip(files[i]);
	}
	return clips.size();
}

UINT GameAnimationModel::AddClip(wstring file)
{
	AnimationClip* clip = new AnimationClip();
	clip->ReadData(file);

	return AddClip(clip);
}

UINT GameAnimationModel::AddClip(AnimationClip* clip)
{
	clips.push_back(clip);

	return (clips.size() - 1);
}

void GameAnimationModel::RemoveClip(UINT index)
{
	SAFE_DELETE(clips[index]);
	clips.erase(clips.begin() + index);
}

void GameAnimationModel::ClearClips(void)
{
	for (AnimationClip* clip : clips)
		SAFE_DELETE(clip);

	clips.clear();
}

AnimationClip* GameAnimationModel::GetClip(UINT index)
{
	assert(clips.size() > index);
	return clips[index];
}

AnimationBlender* GameAnimationModel::GetBlenderFromBoneName(wstring name)
{
	for (AnimationBlender* blender : blenders)
	{
		if (blender->Name == name)
			return blender;
	}

	return NULL;
}

bool GameAnimationModel::PlayAnim(UINT index, AnimationPlayMode mode)
{
	return PlayAnim(index, 0.0f, 0.0f, 1.0f, mode);
}

bool GameAnimationModel::PlayAnim(UINT index, float startTime, float blendTime, float timeScaleFactor, AnimationPlayMode mode)
{
	AnimationClip* clip = GetClip(index);
	if (clip == NULL) 
		return false;

	for (AnimationKeyframe* keyframe : clip->Keyframes)
	{
		AnimationBlender* blender = GetBlenderFromBoneName(keyframe->BoneName);

		blender->AddKeyframe(keyframe, startTime, blendTime, timeScaleFactor, mode);
	}

	return false;
}

float GameAnimationModel::GetPlayTime(wstring boneName)
{
	for (AnimationBlender* blender : blenders)
	{
		if (blender->Name == boneName)
			return blender->Current->PlayTime;
	}

	return 0.0f;
}

