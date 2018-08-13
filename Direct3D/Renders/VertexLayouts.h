#pragma once

//=======================================================================================

struct Vertex
{
	Vertex()
		: position(0, 0, 0) {}

	D3DXVECTOR3 position;
};

//=======================================================================================

struct VertexColor
{
	VertexColor()
		: position(0, 0, 0)
		, color(0, 0, 0, 1) {}

	VertexColor(D3DXVECTOR3	position, D3DXCOLOR color)
		: position(position), color(color) {}

	D3DXVECTOR3	position;
	D3DXCOLOR color;
};

//=======================================================================================

struct VertexColorNormal
{
	VertexColorNormal()
		: position(0, 0, 0)
		, color(0, 0, 0, 1)
		, normal(0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXCOLOR color;
	D3DXVECTOR3 normal;
};

//=======================================================================================

struct VertexTexture
{
	VertexTexture()
		: position(0, 0, 0)
		, uv(0, 0) {}

	D3DXVECTOR3	position;
	D3DXVECTOR2	uv;
};

//=======================================================================================

struct VertexTextureNormal
{
	VertexTextureNormal()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
};

//=======================================================================================

struct VertexColorTextureNormal
{
	VertexColorTextureNormal()
		: position(0, 0, 0)
		, color(0, 0, 0, 1)
		, uv(0, 0)
		, normal(0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXCOLOR color;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
};

//=======================================================================================

struct VertexTextureNormalBlend
{
	VertexTextureNormalBlend()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, blendIndices(0, 0, 0, 0)
		, blendWeights(0, 0, 0, 0) {}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
	D3DXVECTOR4 blendIndices;
	D3DXVECTOR4 blendWeights;
};

//=======================================================================================

struct VertexTextureNormalTangent
{
	VertexTextureNormalTangent()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, tangent(0, 0, 0)
	{}

	D3DXVECTOR3	position;
	D3DXVECTOR2	uv;
	D3DXVECTOR3	normal;
	D3DXVECTOR3 tangent;
};

struct VertexTextureNormalTangentBlend
{
	VertexTextureNormalTangentBlend()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, tangent(0, 0, 0)
		, blendIndices(0, 0, 0, 0)
		, blendWeights(0, 0, 0, 0)
	{}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXVECTOR3 normal;
	D3DXVECTOR4 blendIndices;
	D3DXVECTOR4 blendWeights;
	D3DXVECTOR3 tangent;
};



struct VertexTextureColor2
{
	VertexTextureColor2()
		: position(0, 0, 0)
		, uv(0, 0)
		, color(0, 0, 0, 1)
		, color2(0, 0, 0, 1)
	{}

	D3DXVECTOR3 position;
	D3DXVECTOR2 uv;
	D3DXCOLOR color;
	D3DXCOLOR color2;
};

//=======================================================================================

struct VertexTypePTNC2
{
	VertexTypePTNC2()
		: position(0, 0, 0)
		, uv(0, 0)
		, normal(0, 0, 0)
		, alpha1(0, 0, 0, 1)
		, alpha2(0, 0, 0, 1)
	{}

	VertexTypePTNC2(const D3DXVECTOR3& position
		, const D3DXVECTOR2& uv
		, const D3DXVECTOR3& normal
		, const D3DXCOLOR& alpha1
		, const D3DXCOLOR& alpha2
	)
		: position(position)
		, uv(uv)
		, normal(normal)
		, alpha1(alpha1)
		, alpha2(alpha2)
	{}

	D3DXVECTOR3		position;
	D3DXVECTOR2		uv;
	D3DXVECTOR3		normal;
	D3DXCOLOR		alpha1;
	D3DXCOLOR		alpha2;

	//static D3D11_INPUT_ELEMENT_DESC desc[];
	//static UINT      count;
};