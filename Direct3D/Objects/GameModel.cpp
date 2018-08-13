#include "stdafx.h"
#include "GameModel.h"

GameModel::GameModel(wstring file)
	: velocity(0, 0, 0)
	, shader(NULL)
{
	matFile = file + L".material";
	meshFile = file + L".mesh";

	model = new Model();
	model->ReadMaterial(matFile);
	model->ReadMesh(meshFile);


	//TODO : 테스트용으로 임시 주석
	//shader = new Shader(Shaders + L"999_BoneAnimation.hlsl", "VS");
	shader = new Shader(Shaders + L"999_BoneAnimation.hlsl", "VS_Bone");
	for (Material* material : model->GetMaterials())
	{
		material->SetShader(shader);
	}

	boneBuffer	 = new BoneBuffer();
	renderBuffer = new RenderBuffer();

	D3DXMatrixIdentity(&rotateMatrix);
}

GameModel::~GameModel()
{
	SAFE_DELETE(boneBuffer);
	SAFE_DELETE(renderBuffer);
	SAFE_DELETE(shader);
	SAFE_DELETE(model);
}

void GameModel::Update(ModelShape* modelShape)
{
	//curling

	CalcPosition();

	D3DXMATRIX transformed = Transformed();
	model->CopyAbsoluteBoneTo(transformed, boneTransforms);
}

void GameModel::Render(void)
{
	if (GetVisible() == false)
		return;

	if (boneTransforms.size() > 0)
	{
		boneBuffer->SetBones(&boneTransforms[0], boneTransforms.size());
		boneBuffer->SetVSBuffer(2);

		for (ModelMesh* mesh : model->GetMeshesRef())
		{
			int index = mesh->GetParentBoneIndex();
			renderBuffer->Data.BoneNumber = index;
			renderBuffer->SetVSBuffer(3);

			mesh->Render();
		}
	}
	//TODO : Bone 없을 때 테스트
	else
	{
		for (ModelMesh* mesh : model->GetMeshesRef())
		{
			mesh->Render();
		}
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

