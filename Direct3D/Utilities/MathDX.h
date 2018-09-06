#pragma once

class MathDX
{
public:
	static const D3DXVECTOR3 Vector3MinValue;
	static const D3DXVECTOR3 Vector3MaxValue;

	static const D3DXCOLOR Red;
	static const D3DXCOLOR Green;
	static const D3DXCOLOR Blue;
	static const D3DXCOLOR Yellow;
	static const D3DXCOLOR Aqua;
	static const D3DXCOLOR Magenta;
	static const D3DXCOLOR White;
	static const D3DXCOLOR Black;

	static const D3DXMATRIX matIdentity;

	static void GetMinValue(IN D3DXVECTOR3& vec1, OUT D3DXVECTOR3& vec2);
	static void GetMaxValue(IN D3DXVECTOR3& vec1, OUT D3DXVECTOR3& vec2);

	static bool DecomposeTransform(D3DXMATRIX& mat, D3DXVECTOR3& vs, D3DXVECTOR3& vr, D3DXVECTOR3& vt);
	static void DecomposeTransform(D3DXMATRIX& mat, D3DXVECTOR3& vr, D3DXVECTOR3& vt);

	static float Distance(D3DXVECTOR3& p1, D3DXVECTOR3& p2);
	static float DistanceSquared(D3DXVECTOR3& p1, D3DXVECTOR3& p2);

	static D3DXVECTOR3 GetForward(D3DXMATRIX m);
	static D3DXVECTOR3 GetRight(D3DXMATRIX m);
	static D3DXVECTOR3 GetUp(D3DXMATRIX m);
	static D3DXVECTOR3 GetPosition(D3DXMATRIX m);
};