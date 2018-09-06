#pragma once
#include "AnimationKeyframe.h"

struct AnimationBinder
{
	AnimationPlayMode PlayMode = AnimationPlayMode::Once;

	wstring clipName = L"";
	float Speed = 0.0f;
	float PlayTime = 0.0f;

	AnimationKeyframe* Keyframe;

	float GetDuration() { return Keyframe->Duration; }
	bool IsPlayDone() { return (PlayMode == AnimationPlayMode::Once) && (PlayTime >= Keyframe->Duration); }

	void BindKeyframe(wstring clipName, AnimationKeyframe* keyframe, float start, float speed, AnimationPlayMode mode)
	{
		this->clipName = clipName;
		Keyframe = keyframe;

		PlayTime = start;
		Speed = speed;
		PlayMode = mode;
	}

	void Initialize()
	{
		clipName = L"";
		Keyframe = NULL;

		PlayTime = 0.0f;
		Speed = 1.0f;
		PlayMode = AnimationPlayMode::Repeat;
	}

	void CopyTo(AnimationBinder* clone)
	{
		clone->clipName = this->clipName;
		clone->Keyframe = this->Keyframe;
		clone->PlayTime = this->PlayTime;
		clone->Speed = this->Speed;
		clone->PlayMode = this->PlayMode;
	}

	AnimationKeyframeData GetKeyframeData(float time)
	{
		assert(Keyframe != NULL);

		if (Speed == 0.0f)
			return AnimationKeyframeData();


		PlayTime += Speed * time;
		if (PlayMode == AnimationPlayMode::Once)
		{
			if (PlayTime >= Keyframe->Duration)
				PlayTime = Keyframe->Duration;
		}
		else if (PlayMode == AnimationPlayMode::Repeat)
		{
			if (PlayTime >= Keyframe->Duration)
				PlayTime = Math::Modulo(PlayTime, Keyframe->Duration);
		}

		return Keyframe->GetInterpolateKeyframe(PlayTime, PlayMode);
	}

	D3DXMATRIX GetKeyframeMatrix(float time)
	{
		assert(Keyframe != NULL);

		if (Speed == 0.0f)
			return D3DXMATRIX();


		PlayTime += Speed * time;
		if (PlayMode == AnimationPlayMode::Once)
		{
			if (PlayTime >= Keyframe->Duration)
				PlayTime = Keyframe->Duration;
		}
		else if (PlayMode == AnimationPlayMode::Repeat)
		{
			if (PlayTime >= Keyframe->Duration)
				PlayTime = Math::Modulo(PlayTime, Keyframe->Duration);
		}

		return Keyframe->GetInterpolateMatrix(PlayTime, PlayMode);
	}
};