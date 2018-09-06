#pragma once
#include "AnimationBinder.h"

struct AnimationBlender
{
	AnimationBinder* Current = NULL;
	AnimationBinder* Next = NULL;

	wstring Name = L"";

	UINT  BindCount   = 0;
	float BlendTime   = 0.0f;
	float ElapsedTime = 0.0f;

	bool Empty() { return (BindCount == 0); }
	bool Exist() { return (Empty() == false && Current != NULL); }

	AnimationBlender()
	{
		Current = new AnimationBinder();
		Next = new AnimationBinder();
	}
	~AnimationBlender()
	{
		SAFE_DELETE(Current);
		SAFE_DELETE(Next);
	}

	void ResetBinder()
	{
		Current->Initialize();
		Next->Initialize();

		BindCount = 0;
	}

	void ShiftBinder()
	{
		Current->Initialize();
		Next->CopyTo(Current);
		Next->Initialize();

		BindCount--;
	}

	void AddKeyframe(wstring clipName, AnimationKeyframe* keyframe, float startTime, float blendTime, float timeScaleFactor, AnimationPlayMode mode)
	{
		BlendTime = blendTime;
		ElapsedTime = startTime;

		if (BlendTime == 0.0f)
		{
			ResetBinder();

			Current->BindKeyframe(clipName, keyframe, startTime, timeScaleFactor, mode);
		}
		else
		{
			// 이미 2개가 있으면 시프트해서 하나를 빼준다.?
			if (BindCount == 2)
				ShiftBinder();

			if (BindCount == 0)
				Current->BindKeyframe(clipName, keyframe, startTime, timeScaleFactor, mode);
			else if (BindCount == 1)
				Next->BindKeyframe(clipName, keyframe, startTime, timeScaleFactor, mode);
		}

		BindCount++;
	}

	//TODO : 임시코드
	D3DXMATRIX Negative()
	{
		//D3DXMATRIX scaleX;
		//D3DXMatrixScaling(&scaleX, -1, 1, 1);
		//
		//D3DXMATRIX rotationX;
		//D3DXMatrixRotationX(&rotationX, -Math::ToRadian(90));
		//
		//D3DXMATRIX rotationY;
		//D3DXMatrixRotationY(&rotationY, Math::ToRadian(180));
		//
		//return scaleX * rotationX * rotationY;

		//Scale(-1, 1, 1)		RotX(-90)			RotY(180)			
		//	-1	0	0	0		1	0	0	0		-1	0	0	0
		//	0	1	0	0		0	0	-1	0		0	1	0	0
		//	0	0	1	0		0	1	0	0		0	0	-1	0
		//	0	0	0	1		0	0	0	1		0	0	0	1
		//														
		//	-1	0	0	0		-1	0	0	0					
		//	0	0	-1	0		0	1	0	0					
		//	0	1	0	0		0	0	-1	0					
		//	0	0	0	1		0	0	0	1					
		//														
		//	1	0	0	0										
		//	0	0	1	0										
		//	0	1	0	0										
		//	0	0	0	1										

		return D3DXMATRIX(
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1
		);
	}

	D3DXMATRIX GetKeyframeMatrix(float time)
	{
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);

		assert(BindCount > 0);

		ElapsedTime += time;
		if (BindCount == 1)
		{
			matrix = Current->GetKeyframeMatrix(time);
		}
		else
		{
			float t = ElapsedTime / BlendTime;
			if (t > 1.0f)
			{
				matrix = Next->GetKeyframeMatrix(time);

				ShiftBinder();
			}
			else
			{
				AnimationKeyframeData target;

				AnimationKeyframeData src[2];
				src[0] = Current->GetKeyframeData(time);
				src[1] = Next->GetKeyframeData(time);

				CalcBlendingKeyframe(&target, src, t);

				matrix = CreateTransformFromTarget(target);
			}
		}

		//XNA 아니면 그냥 matrix 로..
		return matrix;
		//return Negative() * matrix * Negative();
	}

	void CalcBlendingKeyframe(AnimationKeyframeData* target, AnimationKeyframeData* src, float t)
	{
		//Translation
		if (src[0].bHasTranslation && src[1].bHasTranslation)
		{
			D3DXVec3Lerp(&target->Translation, &src[0].Translation, &src[1].Translation, t);
			target->bHasTranslation = true;
		}
		else if (src[0].bHasTranslation)
		{
			target->Translation = src[0].Translation;
			target->bHasTranslation = true;
		}
		else if (src[1].bHasTranslation)
		{
			target->Translation = src[1].Translation;
			target->bHasTranslation = true;
		}


		//Scale
		if (src[0].bHasScale && src[1].bHasScale)
		{
			D3DXVec3Lerp(&target->Scale, &src[0].Scale, &src[1].Scale, t);
			target->bHasScale = true;
		}
		else if (src[0].bHasScale)
		{
			target->Scale = src[0].Scale;
			target->bHasScale = true;
		}
		else if (src[1].bHasScale)
		{
			target->Scale = src[1].Scale;
			target->bHasScale = true;
		}


		//Rotation
		if (src[0].bHasRotation && src[1].bHasRotation)
		{
			D3DXQuaternionSlerp(&target->Rotation, &src[0].Rotation, &src[1].Rotation, t);
			target->bHasRotation = true;
		}
		else if (src[0].bHasRotation)
		{
			target->Rotation = src[0].Rotation;
			target->bHasRotation = true;
		}
		else if (src[1].bHasRotation)
		{
			target->Rotation = src[1].Rotation;
			target->bHasRotation = true;
		}
	}

	D3DXMATRIX CreateTransformFromTarget(AnimationKeyframeData target)
	{
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);

		if (target.bHasRotation)
		{
			D3DXMatrixRotationQuaternion(&matrix, &target.Rotation);
		}

		if (target.bHasTranslation)
		{
			matrix._41 = target.Translation.x;
			matrix._42 = target.Translation.y;
			matrix._43 = target.Translation.z;
		}

		if (target.bHasScale)
		{
			D3DXMATRIX temp;
			D3DXMatrixScaling(&temp, target.Scale.x, target.Scale.y, target.Scale.z);

			matrix = matrix * temp;
		}

		return matrix;
	}
};