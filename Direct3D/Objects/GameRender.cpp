#include "stdafx.h"
#include "GameRender.h"


GameRender::GameRender()
	: enable(true), visible(true)
	, position(0, 0, 0)
	, direction(0, 0, 1)
	, up(0, 1, 0)
	, right(1, 0, 0)
{
	D3DXMatrixIdentity(&rootAxis);
	D3DXMatrixIdentity(&world);
}

GameRender::~GameRender()
{
}

void GameRender::SetEnable(bool val) 
{ 
	enable = val; 
	for (auto temp : Enableds) 
		temp(); 
}

bool GameRender::GetEanble(void)
{ 
	return enable; 
}

void GameRender::SetVisible(bool val)
{ 
	visible = val; 
	for (auto temp : Visibled) 
		temp(); 
}

bool GameRender::GetVisible(void) 
{ 
	return visible; 
}

void GameRender::GetRootAxis(D3DXMATRIX& mat)
{
	mat = rootAxis;
}

void GameRender::SetRootAxis(D3DXMATRIX& mat)
{ 
	rootAxis = mat; 
}

void GameRender::GetWorld(D3DXMATRIX& mat)
{ 
	mat = world; 

}

void GameRender::SetWorld(D3DXMATRIX& mat)
{
	world = mat;

	right		= D3DXVECTOR3(world._11, world._12, world._13);
	up			= D3DXVECTOR3(world._21, world._22, world._23);
	direction	= D3DXVECTOR3(world._31, world._32, world._33);
	position	= D3DXVECTOR3(world._41, world._42, world._43);
}

void GameRender::GetPosition(D3DXVECTOR3& vec)
{
	vec = position;
}

void GameRender::SetPosition(D3DXVECTOR3& vec)
{
	position = vec;

	world._41 = position.x;
	world._42 = position.y;
	world._43 = position.z;
}

void GameRender::GetDirection(D3DXVECTOR3& vec)
{
	vec = direction;
}

void GameRender::SetDirection(D3DXVECTOR3& vec)
{
	direction = vec;

	world._31 = direction.x;
	world._32 = direction.y;
	world._33 = direction.z;
}

void GameRender::GetUp(D3DXVECTOR3& vec)
{
	vec = up;
}

void GameRender::SetUp(D3DXVECTOR3& vec)
{
	up = vec;

	world._21 = up.x;
	world._22 = up.y;
	world._23 = up.z;
}

void GameRender::GetRight(D3DXVECTOR3& vec)
{
	vec = right;
}

void GameRender::SetRight(D3DXVECTOR3& vec)
{
	right = vec;

	world._11 = right.x;
	world._12 = right.y;
	world._13 = right.z;
}


D3DXMATRIX GameRender::Transformed(void)
{
	return rootAxis * world;
}
