#include "stdafx.h"
#include "Orthographic.h"

Orthographic::Orthographic(float left, float right, float bottom, float top, float zn, float zf)
{
	Set(left, right, bottom, top, zn, zf);
}

Orthographic::~Orthographic()
{
	
}

void Orthographic::GetMatrix(D3DXMATRIX* mat)
{
	memcpy(mat, &matrix, sizeof(D3DXMATRIX));
}

void Orthographic::Set(float left, float right, float bottom, float top, float zn, float zf)
{
	this->left = left;
	this->right = right;
	this->bottom = bottom;
	this->top = top;
	this->zn = zn;
	this->zf = zf;

	//D3DXMatrixOrthoLH, RH : 중앙점 없음
	D3DXMatrixOrthoOffCenterLH(&matrix, left, right, bottom, top , zn, zf);
}

