#include "stdafx.h"
#include "ModelShape.h"

#include "BoundingBox.h"
#include "BoundingFrustum.h"
#include "BoundingSphere.h"
#include "BoundingCapsule.h"
#include "Ray.h"
#include "Plane.h"

#include "../../Utilities/Math.h"

ModelShape* ModelShape::instance = NULL;


ModelShape::ModelShape()
	: vertexBuffer(NULL), indexBuffer(NULL)
	, isDraging(false)
{
	worldBuffer = new WorldBuffer();

	shader = new Shader(Shaders + L"ShapeRender.hlsl");
	shader2 = new Shader(Shaders + L"ShapeRender.hlsl", "VS_DRAG", "PS_DRAG");

	InitializeDragRect();
}

ModelShape::~ModelShape()
{
	for (int i = 0; i < 2; i++)
	{
		SAFE_RELEASE(blendState[i]);
	}

	SAFE_RELEASE(vertexBuffer2);
	SAFE_RELEASE(indexBuffer2);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
	SAFE_DELETE(shader2);
}

void ModelShape::PreRender(void)
{
}

void ModelShape::Render(void)
{
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	//IA : InputAssembly
	gDC->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	gDC->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	worldBuffer->SetVSBuffer(1);
	shader->Render();

	gDC->DrawIndexed(indexData.size(), 0, 0);

	if (isDraging)
	{
		gDC->OMSetBlendState(blendState[1], NULL, 0xFF);

		gDC->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
		gDC->IASetIndexBuffer(indexBuffer2, DXGI_FORMAT_R32_UINT, 0);
		gDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		shader2->Render();

		gDC->DrawIndexed(8, 0, 0);

		gDC->OMSetBlendState(blendState[0], NULL, 0xFF);

		isDraging = false;
	}
}

void ModelShape::PostRender(void)
{
}

void ModelShape::ResizeScreen(void)
{
}

void ModelShape::ClearBuffer(void)
{
	while (activeShapes.size() > 0)
	{
		SAFE_DELETE(activeShapes[0]);
		activeShapes.erase(activeShapes.begin());
	}

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void ModelShape::CreateBuffer(void)
{
	// Calculate the total number of vertices we're going to be rendering.
	UINT vertexCount = 0;
	for (auto shape : activeShapes)
		vertexCount += shape->LineCount * 2;

	if (vertexCount == 0)
		return;

	// Make sure our array is large enough
	if (vertexData.size() != vertexCount)
	{
		// If we have to resize, we make our array twice as large as necessary so
		// we hopefully won't have to resize it for a while.
		vertexData.resize(vertexCount);
		indexData.resize(vertexCount);
	}

	// Now go through the shapes again to move the vertices to our array and
	// add up the number of lines to draw.
	int lineCount = 0;
	int vertIndex = 0;
	for (DebugShape* shape : activeShapes)
	{
		lineCount += shape->LineCount;
		UINT shapeVerts = shape->LineCount * 2;
		for (UINT i = 0; i < shapeVerts; i++)
			vertexData[vertIndex++] = shape->Vertices[i];
	}
	for (UINT i = 0; i < vertexCount; i++)
	{
		indexData[i] = i;
	}



	//버텍스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertexData.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertexData[0];

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//인덱스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexData.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indexData[0];

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void ModelShape::SetDragRect(RECT rcDrag, D3DXCOLOR* color)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	// x, y, z를  -1~1 사이로 정규화 -> PS 에서 직접 사용할 좌표
	rcDrag.top    = (LONG)desc.Height - rcDrag.top;
	rcDrag.bottom = (LONG)desc.Height - rcDrag.bottom;

	float left   = ((float)rcDrag.left   * 2.0f) / desc.Width  - 1.0f;
	float right  = ((float)rcDrag.right  * 2.0f) / desc.Width  - 1.0f;
	float top    = ((float)rcDrag.top    * 2.0f) / desc.Height - 1.0f;
	float bottom = ((float)rcDrag.bottom * 2.0f) / desc.Height - 1.0f;

	vertexData2[0].position = D3DXVECTOR3(left,  bottom, 0);
	vertexData2[1].position = D3DXVECTOR3(left,  top, 0);
	vertexData2[2].position = D3DXVECTOR3(right, bottom, 0);
	vertexData2[3].position = D3DXVECTOR3(right, top, 0);

	if (color != NULL)
	{
		vertexData2[0].color = *color;
		vertexData2[1].color = *color;
		vertexData2[2].color = *color;
		vertexData2[3].color = *color;
	}

	gDC->UpdateSubresource
	(
		vertexBuffer2, 0, NULL, vertexData2, sizeof(VertexColor) * 4, 0
	);

	isDraging = true;
}



void ModelShape::AddLine(D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXCOLOR color)
{
	// Get a DebugShape we can use to draw the line
	DebugShape* shape = GetShapeForLines(1);

	// Add the two vertices to the shape
	shape->Vertices[0] = VertexColor(a, color);
	shape->Vertices[1] = VertexColor(b, color);
}

void ModelShape::AddTriangle(D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c, D3DXCOLOR color)
{
	// Get a DebugShape we can use to draw the triangle
	DebugShape* shape = GetShapeForLines(3);

	// Add the vertices to the shape
	shape->Vertices[0] = VertexColor(a, color);
	shape->Vertices[1] = VertexColor(b, color);
	shape->Vertices[2] = VertexColor(b, color);
	shape->Vertices[3] = VertexColor(c, color);
	shape->Vertices[4] = VertexColor(c, color);
	shape->Vertices[5] = VertexColor(a, color);
}

void ModelShape::AddBoundingFrustum(BoundingFrustum* frustum, D3DXCOLOR color)
{
	// Get a DebugShape we can use to draw the frustum
	DebugShape* shape = GetShapeForLines(12);

	// Get the corners of the frustum
	vector<D3DXVECTOR3> corners = frustum->GetCorners();

	// Fill in the vertices for the bottom of the frustum
	shape->Vertices[0]  = VertexColor(corners[0], color);
	shape->Vertices[1]  = VertexColor(corners[1], color);
	shape->Vertices[2]  = VertexColor(corners[1], color);
	shape->Vertices[3]  = VertexColor(corners[2], color);
	shape->Vertices[4]  = VertexColor(corners[2], color);
	shape->Vertices[5]  = VertexColor(corners[3], color);
	shape->Vertices[6]  = VertexColor(corners[3], color);
	shape->Vertices[7]  = VertexColor(corners[0], color);

	// Fill in the vertices for the top of the frustum
	shape->Vertices[8]  = VertexColor(corners[4], color);
	shape->Vertices[9]  = VertexColor(corners[5], color);
	shape->Vertices[10] = VertexColor(corners[5], color);
	shape->Vertices[11] = VertexColor(corners[6], color);
	shape->Vertices[12] = VertexColor(corners[6], color);
	shape->Vertices[13] = VertexColor(corners[7], color);
	shape->Vertices[14] = VertexColor(corners[7], color);
	shape->Vertices[15] = VertexColor(corners[4], color);

	// Fill in the vertices for the vertical sides of the frustum
	shape->Vertices[16] = VertexColor(corners[0], color);
	shape->Vertices[17] = VertexColor(corners[4], color);
	shape->Vertices[18] = VertexColor(corners[1], color);
	shape->Vertices[19] = VertexColor(corners[5], color);
	shape->Vertices[20] = VertexColor(corners[2], color);
	shape->Vertices[21] = VertexColor(corners[6], color);
	shape->Vertices[22] = VertexColor(corners[3], color);
	shape->Vertices[23] = VertexColor(corners[7], color);
}

void ModelShape::AddBoundingBox(BoundingBox* box, D3DXCOLOR color)
{
	// Get a DebugShape we can use to draw the box
	DebugShape* shape = GetShapeForLines(12);

	// Get the corners of the box
	vector<D3DXVECTOR3> corners;
	box->GetCorners(corners);

	// Fill in the vertices for the bottom of the box
	shape->Vertices[0]  = VertexColor(corners[0], color);
	shape->Vertices[1]  = VertexColor(corners[1], color);
	shape->Vertices[2]  = VertexColor(corners[1], color);
	shape->Vertices[3]  = VertexColor(corners[2], color);
	shape->Vertices[4]  = VertexColor(corners[2], color);
	shape->Vertices[5]  = VertexColor(corners[3], color);
	shape->Vertices[6]  = VertexColor(corners[3], color);
	shape->Vertices[7]  = VertexColor(corners[0], color);

	// Fill in the vertices for the top of the box
	shape->Vertices[8]  = VertexColor(corners[4], color);
	shape->Vertices[9]  = VertexColor(corners[5], color);
	shape->Vertices[10] = VertexColor(corners[5], color);
	shape->Vertices[11] = VertexColor(corners[6], color);
	shape->Vertices[12] = VertexColor(corners[6], color);
	shape->Vertices[13] = VertexColor(corners[7], color);
	shape->Vertices[14] = VertexColor(corners[7], color);
	shape->Vertices[15] = VertexColor(corners[4], color);

	// Fill in the vertices for the vertical sides of the box
	shape->Vertices[16] = VertexColor(corners[0], color);
	shape->Vertices[17] = VertexColor(corners[4], color);
	shape->Vertices[18] = VertexColor(corners[1], color);
	shape->Vertices[19] = VertexColor(corners[5], color);
	shape->Vertices[20] = VertexColor(corners[2], color);
	shape->Vertices[21] = VertexColor(corners[6], color);
	shape->Vertices[22] = VertexColor(corners[3], color);
	shape->Vertices[23] = VertexColor(corners[7], color);
}

void ModelShape::AddBoundingSphere(BoundingSphere* sphere, D3DXCOLOR color)
{
	vector<D3DXVECTOR3> vertices;
	sphere->GetCorners(vertices);
	// Get a DebugShape we can use to draw the sphere
	DebugShape* shape = GetShapeForLines(vertices.size());

	// Iterate our unit sphere vertices
	for (UINT i = 0; i < vertices.size(); i++)
	{
		shape->Vertices[i] = VertexColor(vertices[i], color);
	}
}

void ModelShape::AddBoundingCapsule(BoundingCapsule* capsule, D3DXCOLOR color)
{
	vector<D3DXVECTOR3> vertices;
	capsule->GetCorners(vertices);
	// Get a DebugShape we can use to draw the sphere
	DebugShape* shape = GetShapeForLines(vertices.size());

	// Iterate our unit sphere vertices
	for (UINT i = 0; i < vertices.size(); i++)
	{
		shape->Vertices[i] = VertexColor(vertices[i], color);
	}
}



void ModelShape::InitializeDragRect(void)
{
	VertexColor vc;
	vc.position = D3DXVECTOR3(0, 0, 0);
	vc.color = D3DXCOLOR(0, 0.7f, 0, 0.5f);


	vertexData2[0] = vc;
	vertexData2[1] = vc;
	vertexData2[2] = vc;
	vertexData2[3] = vc;

	// 1----3
	// |    |
	// 0----2

	indexData2[0] = 0;
	indexData2[1] = 1;
	indexData2[2] = 1;
	indexData2[3] = 3;
	indexData2[4] = 3;
	indexData2[5] = 2;
	indexData2[6] = 2;
	indexData2[7] = 0;

	//버텍스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertexData2[0];

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &vertexBuffer2);
		assert(SUCCEEDED(hr));
	}

	//인덱스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * 8;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indexData2[0];

		HRESULT hr;
		hr = gDevice->CreateBuffer(&desc, &data, &indexBuffer2);
		assert(SUCCEEDED(hr));
	}

	//BlendState
	{
		D3D11_BLEND_DESC desc;
		States::GetBlendDesc(&desc);
		States::CreateBlend(&desc, &blendState[0]);

		desc.AlphaToCoverageEnable = true;
		desc.IndependentBlendEnable = true;
		
		desc.RenderTarget[0].BlendEnable = true;


		States::GetBlendDesc(&desc, 0, BlendMethod::ComposePlus);
		States::CreateBlend(&desc, &blendState[1]);
	}
}

DebugShape* ModelShape::GetShapeForLines(int lineCount)
{
	DebugShape* shape = NULL;

	int vertCount = lineCount * 2;

	shape = new DebugShape(vertCount, lineCount);
	activeShapes.push_back(shape);

	return shape;
}


