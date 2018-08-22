#pragma once
#include "AnimationKeyframe.h"
#include "../Utilities/BinaryFile.h"

struct AnimationClip
{
	float Duration;
	vector<AnimationKeyframe*> Keyframes;

	wstring fileName;
	bool bLoop;

	//inline wstring GetFileName(void) { return fileName; }

	void ReadData(wstring file, bool bLoop = false)
	{
		fileName = file;
		this->bLoop = bLoop;

		//TODO : *.anim 파일 저장 형식 통일해야함
		BinaryReader* r = new BinaryReader();
		r->Open(file);

		UINT keyframeCount = r->UInt(); //Bone 개수
		Duration = r->Float();

		for (UINT i = 0; i < keyframeCount; i++)
		{
			AnimationKeyframe* keyframe = new AnimationKeyframe();

			keyframe->BoneName = String::ToWString(r->String());
			keyframe->FrameCount = r->UInt();
			keyframe->Duration = r->Float();
			keyframe->Interval = r->Float();


			UINT size = 0;

			size = r->UInt();
			if (size > 0)
			{
				//벡터 복사 하지 않고 배열복사하도록
				void* temp = malloc(sizeof(D3DXVECTOR3)*size);
				assert(temp != NULL);
				r->Byte(&temp, sizeof(D3DXVECTOR3)*size);
				D3DXVECTOR3* temp2 = (D3DXVECTOR3*)temp;
				keyframe->Translations.assign(size, D3DXVECTOR3());
				for (size_t i = 0; i < size; i++)
				{
					keyframe->Translations[i] = temp2[i];
				}
				free(temp);
			}

			size = r->UInt();
			if (size > 0)
			{
				void* temp = malloc(sizeof(D3DXQUATERNION)*size);
				assert(temp != NULL);
				r->Byte(&temp, sizeof(D3DXQUATERNION)*size);
				D3DXQUATERNION* temp2 = (D3DXQUATERNION*)temp;
				keyframe->Rotations.assign(size, D3DXQUATERNION());
				for (size_t i = 0; i < size; i++)
				{
					keyframe->Rotations[i] = temp2[i];
				}
				free(temp);
			}

			size = r->UInt();
			if (size > 0)
			{
				void* temp = malloc(sizeof(D3DXVECTOR3)*size);
				assert(temp != NULL);
				r->Byte(&temp, sizeof(D3DXVECTOR3)*size);
				D3DXVECTOR3* temp2 = (D3DXVECTOR3*)temp;
				keyframe->Scales.assign(size, D3DXVECTOR3());
				for (size_t i = 0; i < size; i++)
				{
					keyframe->Scales[i] = temp2[i];
				}
				free(temp);
			}

			size = r->UInt();
			if (size > 0)
			{
				void* temp = malloc(sizeof(float)*size);
				assert(temp != NULL);
				r->Byte(&temp, sizeof(float)*size);
				float* temp2 = (float*)temp;
				keyframe->Times.assign(size, float());
				for (size_t i = 0; i < size; i++)
				{
					keyframe->Times[i] = temp2[i];
				}
				free(temp);
			}

			Keyframes.push_back(keyframe);
		}

		r->Close();
		SAFE_DELETE(r);
	}
};