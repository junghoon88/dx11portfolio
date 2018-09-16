#include "stdafx.h"
#include "GameModel.h"
#include "../Model/Bounding/BoundingCapsule.h"
#include "../Model/Bounding/BoundingBox.h"
#include "../Model/Bounding/BoundingSphere.h"

GameModel::GameModel(wstring file, ANIMATION_TYPE animType)
	: animType(animType)
	, velocity(0, 0, 0)
	, shader(NULL)
	, bPause(false)
	, isPlayEnd(false)
{
	matFile  = file + L".material";
	meshFile = file + L".mesh";

	model = new Model();
	model->ReadMaterial(matFile);
	model->ReadMesh(meshFile);


	//TODO : �׽�Ʈ������ �ӽ� �ּ�
	shader = new Shader(Shaders + L"999_BoneAnimation.hlsl", "VS");
	//shader = new Shader(Shaders + L"999_BoneAnimation.hlsl", "VS_Bone");
	for (Material* material : model->GetMaterials())
	{
		material->SetShader(shader);
	}

	for (UINT i = 0; i < model->GetBoneCount(); i++)
	{
		ModelBone* bone = model->GetBone(i);

		AnimationBlender* blender = new AnimationBlender();
		blender->Name = bone->GetName();

		blenders.push_back(blender);
	}


	boneBuffer	 = new BoneBuffer();
	renderBuffer = new RenderBuffer();

	D3DXMatrixIdentity(&rotateMatrix);
}

GameModel::~GameModel()
{
	ClearClips();

	for (AnimationBlender* blender : blenders)
		SAFE_DELETE(blender);

	SAFE_DELETE(boneBuffer);
	SAFE_DELETE(renderBuffer);
	SAFE_DELETE(shader);
	SAFE_DELETE(model);
}

void GameModel::Update(void)
{
	//curling

	CalcPosition();

	switch (animType)
	{
		case ANIMATION_TYPE::Unknown:
		{
			D3DXMATRIX transformed = Transformed();
			model->CopyAbsoluteBoneTo(transformed, boneTransforms);
		}
		break;
		case ANIMATION_TYPE::Mixamo:
		{
			bool isPlaying = false;
			
			if (clips.size() > 0)
			{
				isPlayEnd = false;

				UINT boneCount = model->GetBoneCount();
				for (UINT i = 0; i < boneCount; i++)
				{
					AnimationBlender* blender = blenders[i];


					if (blender->Exist())
					{
						isPlaying = true;

						float time = (bPause == false) ? Time::Delta() : 0.0f;

						D3DXMATRIX mat = blender->GetKeyframeMatrix(time);

						//�⺻�� X, Z �� �������� �ʵ��� 
						if (i == 0)
						{
							mat._41 = 0;
							mat._43 = 0;
						}

						model->GetBone(i)->SetTransform(mat);

						isPlayEnd |= blender->Current->IsPlayDone();
					}
				}
			}

			D3DXMATRIX transformed = Transformed();
			model->CopyAbsoluteBoneTo(transformed, boneTransforms);

			if (isPlaying)
			{
				UINT boneCount = model->GetBoneCount();
				for (UINT i = 0; i < boneCount; i++)
				{
					//��������ǥ��� �����Ͱ� ���ͼ� �޼���ǥ��� �����ϱ� ����?
					D3DXMATRIX matInvBindPose = model->GetBone(i)->GetAbsTransform();
					D3DXMatrixInverse(&matInvBindPose, NULL, &matInvBindPose);
					boneTransforms[i] = matInvBindPose * boneTransforms[i];
				}
			}
		}
		break;
	}

	//Boundings
	{
		size_t count = boundings.size();
		for (size_t i = 0; i < count; i++)
		{
			boundings[i]->Update();
		}
	}

	gBoundingCollision->AddBoundings(boundings);
}

void GameModel::Render(void)
{
	if (GetVisible() == false)
		return;

	assert(boneTransforms.size() > 0);

	boneBuffer->SetBones(boneTransforms);
	boneBuffer->SetVSBuffer(2);

	for (ModelMesh* mesh : model->GetMeshesRef())
	{
		int index = mesh->GetParentBoneIndex();
		renderBuffer->Data.BoneNumber = index;
		renderBuffer->SetVSBuffer(3);

		mesh->Render();
	}
}

D3DXVECTOR3 GameModel::CalcVelocity(D3DXVECTOR3& vel)
{
	D3DXVECTOR3 v(0, 0, 0);

	if (vel.z != 0.0f)
	{
		D3DXVECTOR3 dir;
		GetDirection(dir);
		v += dir * vel.z;
	}
	if (vel.x != 0.0f)
	{
		D3DXVECTOR3 right;
		GetRight(right);
		v += right * vel.x;
	}
	if (vel.y != 0.0f)
	{
		D3DXVECTOR3 up;
		GetUp(up);
		v += up * vel.y;
	}

	return v;
}

void GameModel::AddPosition(D3DXVECTOR3& vec)
{
	D3DXVECTOR3 pos;
	GetPosition(pos);
	pos += vec;
	SetPosition(pos);
}

void GameModel::SetPosition(D3DXVECTOR3& vec)
{
	GameRender::SetPosition(vec);
}

void GameModel::GetVelocity(D3DXVECTOR3& vec)
{
	vec = velocity;
}

void GameModel::SetVelocity(D3DXVECTOR3& vec)
{
	velocity = vec;
}

void GameModel::Rotate(D3DXVECTOR2 amount)
{
	amount *= Time::Delta();

	D3DXMATRIX RA;
	D3DXVECTOR3 right;
	GetRight(right);
	D3DXMatrixRotationAxis(&RA, &right, Math::ToRadian(amount.y));

	D3DXMATRIX RY;
	D3DXMatrixRotationY(&RY, Math::ToRadian(amount.x));

	rotateMatrix = RA * RY;

	D3DXMATRIX world;
	GetWorld(world);
	SetWorld(rotateMatrix * world);
}

void GameModel::CalcPosition(void)
{
	if (D3DXVec3Length(&velocity) > 0)
	{
		D3DXVECTOR3 v = CalcVelocity(velocity) * Time::Delta();
		AddPosition(v);
	}
}

void GameModel::SetShader(wstring shaderFile)
{
	if (shaderFile.length() == 0)
		return;

	SAFE_DELETE(shader);
	shader = new Shader(shaderFile);
	for (Material* material : model->GetMaterials())
	{
		material->SetShader(shader);
	}
}

UINT GameModel::AddClipAll(wstring path)
{
	vector<wstring> files;
	Path::GetFiles(&files, path, L"*.anim", false);
	for (size_t i = 0; i < files.size(); i++)
	{
		AddClip(files[i]);
	}
	return clips.size();
}

UINT GameModel::AddClip(wstring file, bool bLoop)
{
	AnimationClip* clip = new AnimationClip();
	clip->ReadData(file, bLoop);

	return AddClip(clip);
}

UINT GameModel::AddClip(AnimationClip* clip)
{
	clips.push_back(clip);

	return (clips.size() - 1);
}

UINT GameModel::FindClip(wstring name)
{
	size_t size = clips.size();
	for (size_t i = 0; i < size; i++)
	{
		wstring filename = Path::GetFileNameWithoutExtension(clips[i]->fileName);
		if (filename == name)
		{
			return i;
		}
	}
	return UINT(-1);
}

void GameModel::RemoveClip(UINT index)
{
	SAFE_DELETE(clips[index]);
	clips.erase(clips.begin() + index);
}

void GameModel::ClearClips(void)
{
	for (AnimationClip* clip : clips)
		SAFE_DELETE(clip);

	clips.clear();
}

AnimationClip* GameModel::GetClip(UINT index)
{
	assert(clips.size() > index);
	return clips[index];
}

AnimationBlender* GameModel::GetBlenderFromBoneName(wstring name)
{
	for (AnimationBlender* blender : blenders)
	{
		if (blender->Name == name)
			return blender;
	}

	return NULL;
}

bool GameModel::PlayAnim(UINT index)
{
	return PlayAnim(index, 0.0f, 0.0f, 1.0f);
}

bool GameModel::PlayAnim(UINT index, float startTime, float blendTime, float timeScaleFactor)
{
	AnimationClip* clip = GetClip(index);
	if (clip == NULL)
		return false;

	AnimationPlayMode mode = clip->bLoop == false ? AnimationPlayMode::Once : AnimationPlayMode::Repeat;

	for (AnimationKeyframe* keyframe : clip->Keyframes)
	{
		AnimationBlender* blender = GetBlenderFromBoneName(keyframe->BoneName);

		if (blender == NULL)
			continue;

		wstring clipName = Path::GetFileNameWithoutExtension(clip->fileName);
		blender->AddKeyframe(clipName, keyframe, startTime, blendTime, timeScaleFactor, mode);
	}

	return true;
}

void GameModel::PauseAnim(bool pause)
{
	bPause = pause;
}

void GameModel::StopAnim(void)
{
	UINT boneCount = model->GetBoneCount();
	D3DXMATRIX matI;
	D3DXMatrixIdentity(&matI);

	for (UINT i = 0; i < boneCount; i++)
	{
		AnimationBlender* blender = blenders[i];

		blender->ResetBinder();
		model->GetBone(i)->SetTransform(matI);
	}
}

float GameModel::GetPlayTime(wstring boneName)
{
	for (AnimationBlender* blender : blenders)
	{
		if (blender->Name == boneName)
			return blender->Current->PlayTime;
	}

	return 0.0f;
}

float GameModel::GetPlayProgress(void)
{
	for (AnimationBlender* blender : blenders)
	{
		if (blender->Exist())
		{
			float time = blender->Current->PlayTime;
			float duration = blender->Current->GetDuration();
			return time / duration;
		}
	}
	return 0.0f;
}

bool GameModel::MousePickked(D3DXVECTOR3 start, D3DXVECTOR3 direction, OUT float & dist)
{
	for (ModelMesh* mesh : model->GetMeshesRef())
	{
		int parent = mesh->GetParentBoneIndex();

		D3DXMATRIX matParent;
		D3DXMatrixIdentity(&matParent);
		if(parent >= 0)
			matParent = boneTransforms[parent]; // skintransform ���� �ؾ��ϳ�?
		
		if (mesh->MousePickked(matParent, start, direction, dist))
		{
			return true;
		}
	}
	return false;
}

Bounding* GameModel::GetBounding(string name)
{
	for (size_t i = 0; i < boundings.size(); i++)
	{
		if (boundings[i]->GetName() == name)
		{
			return boundings[i];
		}
	}

	return nullptr;
}




