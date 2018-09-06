#include "stdafx.h"
#include "MathDX.h"

const D3DXVECTOR3 MathDX::Vector3MinValue = D3DXVECTOR3(-3E+38f, -3E+38f, -3E+38f);
const D3DXVECTOR3 MathDX::Vector3MaxValue = D3DXVECTOR3( 3E+38f,  3E+38f,  3E+38f);

const D3DXCOLOR MathDX::Red		= D3DXCOLOR(1, 0, 0, 1);
const D3DXCOLOR MathDX::Green	= D3DXCOLOR(0, 1, 0, 1);
const D3DXCOLOR MathDX::Blue	= D3DXCOLOR(0, 0, 1, 1);
const D3DXCOLOR MathDX::Yellow	= D3DXCOLOR(1, 1, 0, 1);
const D3DXCOLOR MathDX::Aqua	= D3DXCOLOR(0, 1, 1, 1);
const D3DXCOLOR MathDX::Magenta	= D3DXCOLOR(1, 0, 1, 1);
const D3DXCOLOR MathDX::White	= D3DXCOLOR(1, 1, 1, 1);
const D3DXCOLOR MathDX::Black	= D3DXCOLOR(0, 0, 0, 1);

const D3DXMATRIX MathDX::matIdentity = 
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

void MathDX::GetMinValue(IN D3DXVECTOR3 & vec1, OUT D3DXVECTOR3 & vec2)
{
	if (vec2.x > vec1.x)	vec2.x = vec1.x;
	if (vec2.y > vec1.y)	vec2.y = vec1.y;
	if (vec2.z > vec1.z)	vec2.z = vec1.z;
}

void MathDX::GetMaxValue(IN D3DXVECTOR3 & vec1, OUT D3DXVECTOR3 & vec2)
{
	if (vec2.x < vec1.x)	vec2.x = vec1.x;
	if (vec2.y < vec1.y)	vec2.y = vec1.y;
	if (vec2.z < vec1.z)	vec2.z = vec1.z;
}

bool MathDX::DecomposeTransform(D3DXMATRIX & mat, D3DXVECTOR3 & vs, D3DXVECTOR3 & vr, D3DXVECTOR3 & vt)
{
	/*
	가정1. mat 의 회전성분은 YawPitchRoll 로 만들어진 행렬이어야 함! (ZXY)
	가정2. Pitch(-Pi/2~Pi/2), Yaw(-Pi~Pi), Roll(-Pi~Pi) 범위여야 함!
	회전 식 결과는 아래와 같다.
	out._11 = ( cos(r) *  cos(y)) + (sin(r) * sin(p) * sin(y));
	out._12 = ( sin(r) *  cos(p));
	out._13 = ( cos(r) * -sin(y)) + (sin(r) * sin(p) * cos(y));

	out._21 = (-sin(r) * cos(y)) + (cos(r) * sin(p) * sin(y));
	out._22 = ( cos(r) * cos(p));
	out._23 = ( sin(r) * sin(y)) + (cos(r) * sin(p) * cos(y));

	out._31 = ( cos(p) * sin(y));
	out._32 = (-sin(p));
	out._33 = ( cos(p) * cos(y));

	out._44 = 1;
	etc = 0;

	ZXY
	| CZ*CY + SX*SY*SZ	CX*SZ	-SY*CZ+SX*CY*SZ		0|
	| -CY*SZ+SX*SY*CZ	CX*CZ	SY*SZ+SX*CY*CZ		0|
	| CX*SY				-SX		CX*CY				0|
	| 0					0		0					1|

	ZX
	|CZ		CX*SZ	SX*SZ	0|
	|-SZ	CX*CZ	SX*CZ	0|
	|0		-SX		CX		0|
	|0		0		0		1|

	*/

	//tan(Z) = _12 / _22
	//tan(Y) = _31 / _33
	//sin(X) = -_32

	//Step0. Translation 구하기
	vt = D3DXVECTOR3(mat._41, mat._42, mat._43);

	D3DXMATRIX* error = NULL; //역행렬 구할떄 에러 체크용.. 나중에 필요하면 
	D3DXMATRIX matSZXY, matSZX, matSZ, matS;
	matSZXY = mat;
	matSZXY._41 = matSZXY._42 = matSZXY._43 = 0;

	//Step1. yaw 구하기
	if (fabs(matSZXY._33) < 1e-6)
	{
		//matSZXY._33 == 0 일경우 -> CX = 0 or CY = 0
		return false;
	}
	else
	{
		/*
		out._31 / out._33 
		= ( cos(p) * sin(y)) / ( cos(p) * cos(y))
		= sin(y) / cos(y) = tan(y)
		*/
		vr.y = atan2(matSZXY._31, matSZXY._33);
	}

	//Step2. ZXY * Y' = ZX 연산
	{
		D3DXMATRIX matY, matY_INV;
		D3DXMatrixRotationY(&matY, vr.y);
		D3DXMatrixInverse(&matY_INV, NULL, &matY);
		matSZX = matSZXY * matY_INV;
		/*
		matSZX
		|CZ		CX*SZ	SX*SZ	0|
		|-SZ	CX*CZ	SX*CZ	0|
		|0		-SX		CX		0|
		|0		0		0		1|
	
		Step2-1. rotX (Pitch) 구하기
		matSZX._23 / matSZX._22 = SX/CX = TX
		*/
		if (fabs(matSZX._22) < 1e-6)
		{
			return false;
		}
		else
		{
			vr.x = atan2(matSZX._23, matSZX._22);
		}
	}


	//Step3. ZX * X' = Z 연산
	{
		D3DXMATRIX matX, matX_INV;
		D3DXMatrixRotationX(&matX, vr.x);
		D3DXMatrixInverse(&matX_INV, NULL, &matX);
		matSZ = matSZX * matX_INV;

		/*
		matSZ
		|CZ		SZ	0	0|
		|-SZ	CZ	0	0|
		|0		0	1	0|
		|0		0	0	1|

		Step3-1. rotZ (Roll) 구하기
		matSZ._12 / matSZ._11 = SZ / CZ = TZ
		*/
		if (fabs(matSZ._11) < 1e-6)
		{
			return false;
		}
		else
		{
			vr.z = atan2(matSZ._12, matSZ._11);
		}
	}

	//Step4. matS 구하기
	{
		D3DXMATRIX matZ, matZ_INV;
		D3DXMatrixRotationZ(&matZ, vr.z);
		D3DXMatrixInverse(&matZ_INV, NULL, &matZ);
		matS = matSZ * matZ_INV;

		vs = D3DXVECTOR3(matS._11, matS._22, matS._33);
	}

	return true;
}

void MathDX::DecomposeTransform(D3DXMATRIX & mat, D3DXVECTOR3 & vr, D3DXVECTOR3 & vt)
{
	/*
	추가가정. mat 의 Scale 값은 1, 1, 1 이다.
	*/

	vr.z = atan2(mat._12, mat._22);
	vr.y = atan2(mat._31, mat._33);
	vr.x = asin(-mat._32);

	vt = D3DXVECTOR3(mat._41, mat._42, mat._43);
}

float MathDX::Distance(D3DXVECTOR3& p1, D3DXVECTOR3& p2)
{
	return D3DXVec3Length(&(p1 - p2));
}

float MathDX::DistanceSquared(D3DXVECTOR3& p1, D3DXVECTOR3& p2)
{
	return D3DXVec3LengthSq(&(p1 - p2));
}

D3DXVECTOR3 MathDX::GetForward(D3DXMATRIX m)
{
	return D3DXVECTOR3(m._31, m._32, m._33);
}

D3DXVECTOR3 MathDX::GetRight(D3DXMATRIX m)
{
	return D3DXVECTOR3(m._11, m._12, m._13);
}

D3DXVECTOR3 MathDX::GetUp(D3DXMATRIX m)
{
	return D3DXVECTOR3(m._21, m._22, m._23);
}

D3DXVECTOR3 MathDX::GetPosition(D3DXMATRIX m)
{
	return D3DXVECTOR3(m._41, m._42, m._43);
}
