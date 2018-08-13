#pragma once
#include "stdafx.h"

enum class AnimationPlayMode
{
	Once = 0, Repeat,
};

struct AnimationKeyframeData
{
	bool bHasTranslation = false;
	bool bHasRotation = false;
	bool bHasScale = false;

	D3DXVECTOR3 Translation;
	D3DXQUATERNION Rotation;
	D3DXVECTOR3 Scale;
};

struct AnimationKeyframe
{
	wstring BoneName = L"";

	UINT FrameCount = 0;

	float Interval = 0.0f;
	float Duration = 0.0f;

	vector<D3DXVECTOR3> Translations;
	vector<D3DXQUATERNION> Rotations;
	vector<D3DXVECTOR3> Scales;
	vector<float> Times;

	inline bool HasTranslation(void)   { return (Translations.size() > 0); }
	inline bool HasRotation(void)      { return (Rotations.size() > 0); }
	inline bool HasScale(void)		   { return (Scales.size() > 0); }
	inline bool HasTime(void)		   { return (Times.size() > 0); }
	inline bool FixedTranslation(void) { return (Translations.size() == 1); }
	inline bool FixedRotation(void)    { return (Rotations.size() == 1); }
	inline bool FixedScale(void)	   { return (Scales.size() == 1); }

private:
	UINT GetKeyframeIndex(float time, AnimationPlayMode mode)
	{
		assert(FrameCount > 0);

		if (FrameCount == 1)
			return 0;

		UINT start, end, middle;
		start = 0;
		end = FrameCount - 1;


		if (HasTime() == false)
		{
			if (mode == AnimationPlayMode::Once)
			{
				UINT index = (UINT)(time / Interval);
				index = (index < end) ? index : end;
				return index;
			}
			else if (mode == AnimationPlayMode::Repeat)
			{
				UINT index = (UINT)(time / Interval);
				index = index % FrameCount;
				return index;
			}
			assert(false);
		}

		if (time >= Times[end] && mode == AnimationPlayMode::Once)
		{
			return end;
		}


		//이진탐색 개념과 비슷
		do
		{
			middle = (start + end) / 2;

			if (end - start <= 1)
			{
				break;
			}
			else if (Times[middle] < time)
			{
				start = middle;
			}
			else if (Times[middle] > time)
			{
				end = middle;
			}
			else
			{
				start = middle;
				break;
			}
		} while ((end - start) > 1);

		return start;
	}

	void CalcKeyframeIndex(float time, AnimationPlayMode mode, OUT UINT& index1, OUT UINT& index2, OUT float& interpolatedTime)
	{
		assert(FrameCount > 0);

		index1 = index2 = 0;
		interpolatedTime = 0.0f;

		if (FrameCount == 1)
		{
			//FrameCount == 1 일 경우 Interval이 0.0f 이므로 예외처리함
			index1 = index2 = 0;
			interpolatedTime = 1.0f;
			return;
		}

		switch (mode)
		{
			case AnimationPlayMode::Once:
			{
				index1 = GetKeyframeIndex(time, mode);

				if (index1 >= FrameCount - 1)
				{
					index1 = index2 = FrameCount - 1;

					interpolatedTime = 1.0f;
				}
				else
				{
					index2 = index1 + 1;

					if (HasTime() == true)
					{
						interpolatedTime = (time - Times[index1]) / (Times[index2] - Times[index1]);
					}
					else
					{
						interpolatedTime = Math::Modulo(time, Interval) / Interval;
					}
				}
			}
			break;
			case AnimationPlayMode::Repeat:
			{
				//애니메이션 마지막 프레임이 짧으면 수정해야함
				time = Math::Modulo(time, Duration);
				index1 = GetKeyframeIndex(time, mode);

				if (index1 >= FrameCount - 1)
				{
					index1 %= FrameCount;
					index2 = (index1 + 1) % FrameCount;
				}
				else
				{
					index2 = (index1 + 1) % FrameCount;
				}

				if (HasTime() == true)
				{
					if (index1 < index2)
					{
						interpolatedTime = (time - Times[index1]) / (Times[index2] - Times[index1]);
					}
					else
					{
						//역전됐을 때에는 처음부터 보여주자
						interpolatedTime = 1.0f;
						//interpolatedTime = (time - Times[index1]) / (Duration + Times[index1] - Times[index2]);
					}
				}
				else
				{
					interpolatedTime = Math::Modulo(time, Interval) / Interval;
				}
			}
			break;
		}
	}


	D3DXMATRIX GetInterpolateMatrix(UINT index1, UINT index2, float t)
	{
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&temp);

		if (HasRotation() == true)
		{
			D3DXQUATERNION q = Rotations[0];

			if (FixedRotation() == false)
			{
				if (index1 == index2)
				{
					q = Rotations[index1];
				}
				else
				{
					D3DXQuaternionSlerp(&q, &Rotations[index1], &Rotations[index2], t);
				}
			}
			D3DXMatrixRotationQuaternion(&temp, &q);
		}

		if (HasTranslation() == true)
		{
			D3DXVECTOR3 translation = Translations[0];

			if (FixedTranslation() == false)
			{
				if (index1 == index2)
				{
					translation = Translations[index1];
				}
				else
				{
					D3DXVec3Lerp(&translation, &Translations[index1], &Translations[index2], t);
				}
			}

			temp._41 = translation.x;
			temp._42 = translation.y;
			temp._43 = translation.z;
		}

		if (HasScale() == true)
		{
			D3DXVECTOR3 scale = Scales[0];

			if (FixedScale() == false)
			{
				scale = Scales[index1];
			}
			else
			{
				D3DXVec3Lerp(&scale, &Scales[index1], &Scales[index2], t);
			}

			D3DXMATRIX matS;
			D3DXMatrixScaling(&matS, scale.x, scale.y, scale.z);

			//TODO : 왜 쌤이 temp * matS 로 했는지 이해가 안간다..
			//D3DXMATRIX temp2 = matS * temp;
			temp = temp * matS;
			//return temp2;
		}

		return temp;
	}

public:
	D3DXMATRIX GetInterpolateMatrix(float time, AnimationPlayMode mode)
	{
		UINT index1 = 0;
		UINT index2 = 0;
		float interpolatedTime = 0.0f;

		CalcKeyframeIndex(time, mode, index1, index2, interpolatedTime);

		return GetInterpolateMatrix(index1, index2, interpolatedTime);
	}

private:
	AnimationKeyframeData GetInterpolateKeyframe(UINT index1, UINT index2, float t)
	{
		AnimationKeyframeData keyframe;

		keyframe.bHasRotation = HasRotation();
		if (keyframe.bHasRotation)
		{
			if (FixedRotation())
			{
				keyframe.Rotation = Rotations[0];
			}
			else
			{
				if (index1 == index2)
				{
					keyframe.Rotation = Rotations[index1];
				}
				else
				{
					D3DXQuaternionSlerp(&keyframe.Rotation, &Rotations[index1], &Rotations[index2], t);
				}
			}
		}
		else
		{
			D3DXQuaternionIdentity(&keyframe.Rotation);
		}


		keyframe.bHasTranslation = HasTranslation();
		if (keyframe.bHasTranslation)
		{
			if (FixedTranslation())
			{
				keyframe.Translation = Translations[0];
			}
			else
			{
				if (index1 == index2)
				{
					keyframe.Translation = Translations[index1];
				}
				else
				{
					D3DXVec3Lerp(&keyframe.Translation, &Translations[index1], &Translations[index2], t);
				}
			}
		}
		else
		{
			keyframe.Translation = D3DXVECTOR3(0, 0, 0);
		}


		keyframe.bHasScale = HasScale();
		if (keyframe.bHasScale)
		{
			if (FixedScale())
			{
				keyframe.Scale = Scales[0];
			}
			else
			{
				if (index1 == index2)
				{
					keyframe.Scale = Scales[index1];
				}
				else
				{
					D3DXVec3Lerp(&keyframe.Scale, &Scales[index1], &Scales[index2], t);
				}
			}
		}
		else
		{
			keyframe.Scale = D3DXVECTOR3(1, 1, 1);
		}

		return keyframe;
	}

public:
	AnimationKeyframeData GetInterpolateKeyframe(float time, AnimationPlayMode mode)
	{
		UINT index1 = 0;
		UINT index2 = 0;
		float interpolateTime = 0.0f;

		CalcKeyframeIndex(time, mode, index1, index2, interpolateTime);

		return GetInterpolateKeyframe(index1, index2, interpolateTime);
	}
};