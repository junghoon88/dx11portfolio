#include "stdafx.h"
#include "ModelAnimPlayer.h"
#include "ModelAnimClip.h"

ModelAnimPlayer::ModelAnimPlayer(Model * model)
	: model(model)
	, mode(Mode::Play)
	, currentKeyframe(0), frameTime(0.0f), keyframeFactor(0.0f)
	, nextKeyframe(0)
	, bUseSlerp(true)
	, bEndAnim(false)
{
	//shader = new Shader(Shaders + L"999_Mesh.hlsl");
	shader = new Shader(Shaders + L"999_Animation.hlsl");

	vector<Material*>& materials = model->GetMaterials();
	for (Material* mat : materials)
	{
		mat->SetShader(shader);
	}

	currentClip = model->GetClip(0);

	ModelBone* root = model->GetBone(1);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	skinTransform.resize(model->GetBoneCount(), mat);
	boneAnimation.resize(model->GetBoneCount(), mat);
}

ModelAnimPlayer::~ModelAnimPlayer()
{
	SAFE_DELETE(shader); //여기서 지우면..
}

void ModelAnimPlayer::Update(D3DXMATRIX& matWorld)
{
	if (currentClip == NULL || mode != Mode::Play)
		return;

	UpdateTime();
	UpdateBone();

	for (size_t i = 0; i < skinTransform.size(); i++)
	{
		skinTransform[i] = skinTransform[i] * matWorld;
	}

	//model->Getbuffer()->SetBones(skinTransform);
}

void ModelAnimPlayer::Render(void)
{
	//model->Getbuffer()->SetVSBuffer(2);

	for (ModelMesh* mesh : model->GetMeshesRef())
		mesh->Render();
}

void ModelAnimPlayer::SelectClip(UINT index, bool useSlerp)
{
	if (currentClip == model->GetClip(index))
		return;

	currentClip = model->GetClip(index);
	
	currentKeyframe = 0;
	frameTime = 0.0f;
	keyframeFactor = 0.0f;
	nextKeyframe = 0;
	bUseSlerp = useSlerp;
	bEndAnim = false;
}

void ModelAnimPlayer::RefreshClip(void)
{
	currentKeyframe = 0;
	frameTime = 0.0f;
	keyframeFactor = 0.0f;
	nextKeyframe = 0;
	bEndAnim = false;
}

bool ModelAnimPlayer::isCurrentClip(UINT index)
{
	if (currentClip == model->GetClip(index))
		return true;

	return false;
}

void ModelAnimPlayer::SetMode(Mode mode)
{
	this->mode = mode;
	switch (mode)
	{
		case ModelAnimPlayer::Mode::Play:
		break;
		case ModelAnimPlayer::Mode::Pause:
		break;
		case ModelAnimPlayer::Mode::Stop:
			currentKeyframe = 0;
			frameTime = 0.0f;
			keyframeFactor = 0.0f;
			nextKeyframe = 0;
			bEndAnim = false;
		break;
	}
}

void ModelAnimPlayer::UpdateTime(void)
{
	frameTime += Time::Delta();

	float invFrameRate = 1.0f / currentClip->GetFrameRate();
	int keyframeCount = currentClip->GetTotalFrame();

	
	if (currentClip->GetLoop())
	{
		while (frameTime > invFrameRate)
		{
			currentKeyframe = (currentKeyframe + 1) % keyframeCount;
			nextKeyframe = (currentKeyframe + 1) % keyframeCount;

			frameTime -= invFrameRate;
		}

		keyframeFactor = frameTime / invFrameRate;
	}
	else
	{
		if (bEndAnim)
			return;

		while (frameTime > invFrameRate)
		{
			currentKeyframe = (currentKeyframe + 1);
			nextKeyframe = (currentKeyframe + 1);

			frameTime -= invFrameRate;
		}
		keyframeFactor = frameTime / invFrameRate;

		//예외처리
		if (currentKeyframe >= keyframeCount - 1)
		{
			currentKeyframe = keyframeCount - 1;
			nextKeyframe = keyframeCount - 1;
			bEndAnim = true;
		}
		keyframeFactor = 0.0f;
	}
}

void ModelAnimPlayer::UpdateBone(void)
{
	UINT boneCount = model->GetBoneCount();
	for (UINT i = 0; i < boneCount; i++)
	{
		ModelBone* bone = model->GetBone(i);

		D3DXMATRIX matAnimation;
		D3DXMATRIX matParentAnimation;

		//오른손좌표계로 데이터가 들어와서 왼손좌표계로 변경하기 위해?
		D3DXMATRIX matInvBindPose = bone->GetAbsTransform();
		D3DXMatrixInverse(&matInvBindPose, NULL, &matInvBindPose);

		ModelKeyframe* frame = currentClip->GetKeyframe(bone->GetName());
		if (frame == NULL) continue;

		if (bUseSlerp)
		{
			D3DXMATRIX S, R, T;

			ModelKeyframeData current = frame->Datas[currentKeyframe];
			ModelKeyframeData next = frame->Datas[nextKeyframe];

			D3DXVECTOR3 s1 = current.Scale;
			D3DXVECTOR3 s2 = next.Scale;

			D3DXVECTOR3 s;
			D3DXVec3Lerp(&s, &s1, &s2, keyframeFactor);
			D3DXMatrixScaling(&S, s.x, s.y, s.z);


			D3DXQUATERNION q1 = current.Rotation;
			D3DXQUATERNION q2 = next.Rotation;

			D3DXQUATERNION q;
			D3DXQuaternionSlerp(&q, &q1, &q2, keyframeFactor);
			D3DXMatrixRotationQuaternion(&R, &q);


			D3DXVECTOR3 t1 = current.Translation;
			D3DXVECTOR3 t2 = next.Translation;

			D3DXVECTOR3 t;
			D3DXVec3Lerp(&t, &t1, &t2, keyframeFactor);
			D3DXMatrixTranslation(&T, t.x, t.y, t.z);

			matAnimation = S * R * T;
		}
		else
		{
			matAnimation = frame->Datas[currentKeyframe].Transform;
		}

		int parentIndex = bone->GetParentIndex();
		if (parentIndex < 0)
			D3DXMatrixIdentity(&matParentAnimation);
		else
			matParentAnimation = boneAnimation[parentIndex];

		boneAnimation[i] = matAnimation * matParentAnimation;
		skinTransform[i] = matInvBindPose * boneAnimation[i];
	}
}
