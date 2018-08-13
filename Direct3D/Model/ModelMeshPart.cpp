#include "stdafx.h"
#include "ModelMeshPart.h"

ModelMeshPart::ModelMeshPart()
{
}

ModelMeshPart::~ModelMeshPart()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void ModelMeshPart::Clone(void** clone)
{
	ModelMeshPart* part = new ModelMeshPart();

	//ModelMesh*				parent;
	part->parent = NULL; //부모에서 

	part->materialName = this->materialName;

	part->vertices.assign(vertices.begin(), vertices.end());
	part->indices.assign(indices.begin(), indices.end());

	*clone = part;
}


void ModelMeshPart::Binding(void)
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ModelVertexType) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertices[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//2. Index Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indices[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void ModelMeshPart::Render(void)
{
	UINT stride = sizeof(ModelVertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (material)
	{
		material->SetBuffer();
	}

	D3D::GetDC()->DrawIndexed(indices.size(), 0, 0);
}

void ModelMeshPart::ScanPointMinMax(D3DXVECTOR3 * min, D3DXVECTOR3 * max, D3DXMATRIX& matWorld)
{
	D3DXVECTOR3 pointMin = D3DXVECTOR3(Math::FloatMaxValue, Math::FloatMaxValue, Math::FloatMaxValue);
	D3DXVECTOR3 pointMax = D3DXVECTOR3(Math::FloatMinValue, Math::FloatMinValue, Math::FloatMinValue);

	for (UINT i = 0; i < vertices.size(); i++)
	{
		D3DXVECTOR3 pos;
		D3DXVec3TransformCoord(&pos, &vertices[i].position, &matWorld);

		if (pointMin.x > pos.x)	pointMin.x = pos.x;
		if (pointMax.x < pos.x)	pointMax.x = pos.x;

		if (pointMin.y > pos.y)	pointMin.y = pos.y;
		if (pointMax.y < pos.y)	pointMax.y = pos.y;

		if (pointMin.z > pos.z)	pointMin.z = pos.z;
		if (pointMax.z < pos.z)	pointMax.z = pos.z;
	}

	if (min->x > pointMin.x)	min->x = pointMin.x;
	if (min->y > pointMin.y)	min->y = pointMin.y;
	if (min->z > pointMin.z)	min->z = pointMin.z;

	if (max->x < pointMax.x)	max->x = pointMax.x;
	if (max->y < pointMax.y)	max->y = pointMax.y;
	if (max->z < pointMax.z)	max->z = pointMax.z;
}

