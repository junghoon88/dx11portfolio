#pragma once
#include "stdafx.h"

class WorldBuffer : public ShaderBuffer
{
public:
	WorldBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		D3DXMatrixIdentity(&Data.World);
	}

	void SetMatrix(D3DXMATRIX& mat)
	{
		Data.World = mat;
		//DX10 ���� HLSL�� ���켱���� �ٲ���� ������ Transpose �� �ؾ��Ѵ�. 
		//View �� ��켱(D3DXMatrixLookAtLH)���� ������� 
		//�� ������ �����Ϸ��� World���� TRS, RH�� ���� �ؾ���.;;
		D3DXMatrixTranspose(&Data.World, &Data.World);
	}

	D3DXMATRIX GetWorld(void)
	{
		return Data.World;
	}

	struct Struct
	{
		D3DXMATRIX World;
	};

private:
	Struct Data;
};

class ViewProjectionBuffer : public ShaderBuffer
{
public:
	ViewProjectionBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		D3DXMatrixIdentity(&Data.View);
		D3DXMatrixIdentity(&Data.Projection);
		D3DXMatrixInverse(&Data.ViewInverse, NULL, &Data.View);
	}

	void SetView(D3DXMATRIX& mat)
	{
		Data.View = mat;

		D3DXMatrixInverse(&Data.ViewInverse, NULL, &Data.View);

		//DX10 ���� HLSL�� ���켱���� �ٲ���� ������ Transpose �� �ؾ��Ѵ�. 
		//View �� ��켱(D3DXMatrixLookAtLH)���� ������� 
		//�� ������ �����Ϸ��� World���� TRS, RH�� ���� �ؾ���.;;
		D3DXMatrixTranspose(&Data.View, &Data.View);
		D3DXMatrixTranspose(&Data.ViewInverse, &Data.ViewInverse);
	}

	void SetProjection(D3DXMATRIX& mat)
	{
		Data.Projection = mat;
		//DX10 ���� HLSL�� ���켱���� �ٲ���� ������ Transpose �� �ؾ��Ѵ�. 
		//Projection �� ��켱(D3DXMatrixPerspectiveFovLH)���� ������� 
		//�� ������ �����Ϸ��� World���� TRS, RH�� ���� �ؾ���.;;
		D3DXMatrixTranspose(&Data.Projection, &Data.Projection);
	}

	struct Struct
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
		D3DXMATRIX ViewInverse;
	};

private:
	Struct Data;
};

class LightBuffer : public ShaderBuffer
{
public:
	LightBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.Direction = D3DXVECTOR3(-1, -1, -1);
		Data.Color = D3DXCOLOR(1, 1, 1, 1);

		Data.Overcast = 1.0f;
		//Data.Intensity;
	}

	struct Struct
	{
		D3DXVECTOR3 Direction;
		float Padding1;

		D3DXCOLOR Color;

		float Overcast;		//�帲
		float Intensity;	//����

		float Padding2[2];
	};

	Struct Data;
};

class TimeBuffer : public ShaderBuffer
{
public:
	TimeBuffer() : ShaderBuffer(&Data, sizeof(Struct))
	{
		Data.ElapsedTime = 0.0f;
		Data.RunningTime = 0.0f;
	}

	struct Struct
	{
		float ElapsedTime;
		float RunningTime;

		float Padding[2];
	};

	Struct Data;
};