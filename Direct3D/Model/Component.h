#pragma once

typedef struct TRANSFORM
{
	D3DXVECTOR3 Scale;
	D3DXVECTOR3 RotationDeg;
	D3DXVECTOR3 Position;

	TRANSFORM()
		: Scale(1, 1, 1)
		, RotationDeg(0, 0, 0)
		, Position(0, 0, 0)
	{
	}

	TRANSFORM(D3DXVECTOR3 s, D3DXVECTOR3 r, D3DXVECTOR3 t)
		: Scale(s)
		, RotationDeg(r)
		, Position(t)
	{
	}

	D3DXMATRIX GetMatrix(void)
	{
		D3DXMATRIX S, R, T;
		D3DXMatrixScaling(&S, Scale.x, Scale.y, Scale.z);
		D3DXVECTOR3 RotRad = Math::ToRadian(RotationDeg);
		D3DXMatrixRotationYawPitchRoll(&R, RotRad.y, RotRad.x, RotRad.z);
		D3DXMatrixTranslation(&T, Position.x, Position.y, Position.z);

		return (S*R*T);
	}

	D3DXMATRIX GetMatrixScaling(void)
	{
		D3DXMATRIX S;
		D3DXMatrixScaling(&S, Scale.x, Scale.y, Scale.z);
		return S;
	}

	D3DXMATRIX GetMatrixRotation(void)
	{
		D3DXMATRIX R;
		D3DXVECTOR3 RotRad = Math::ToRadian(RotationDeg);
		D3DXMatrixRotationYawPitchRoll(&R, RotRad.y, RotRad.x, RotRad.z);
		return R;
	}

	D3DXMATRIX GetMatrixTranslation(void)
	{
		D3DXMATRIX T;
		D3DXMatrixTranslation(&T, Position.x, Position.y, Position.z);
		return T;
	}

	//TRANSFROM
	//inline void SetTransform(TRANSFORM tr)				{ transform = tr; }
	//inline void SetTransformScale(D3DXVECTOR3 vec)		{ transform.Scale = vec; }
	//inline void SetTransformRotateDeg(D3DXVECTOR3 vec)	{ transform.RotationDeg = vec; }
	//inline void SetTransformPosition(D3DXVECTOR3 vec)	{ transform.Position = vec; }
	//inline void SetTransformTranslate(D3DXVECTOR3 vec)	{ transform.Position += vec; }
	//inline TRANSFORM GetTransform(void)					{ return transform; }
	//inline D3DXVECTOR3 GetTransformScale(void)			{ return transform.Scale; }
	//inline D3DXVECTOR3 GetTransformRotateDeg(void)		{ return transform.RotationDeg; }
	//inline D3DXVECTOR3 GetTransformPosition(void)		{ return transform.Position; }
	//inline D3DXVECTOR3& GetTransformScaleRef(void)		{ return transform.Scale; }
	//inline D3DXVECTOR3& GetTransformRotateDegRef(void)	{ return transform.RotationDeg; }
	//inline D3DXVECTOR3& GetTransformPositionRef(void)	{ return transform.Position; }
} TRANSFORM;

typedef struct BOX_COLLIDER
{
	D3DXVECTOR3 Center;
	D3DXVECTOR3 Size;

	D3DXVECTOR3 PointMax;
	D3DXVECTOR3 PointMin;

	D3DXCOLOR color;

	vector<VertexColor> vertexData;
	vector<UINT>		indexData;

	ID3D11Buffer*		vertexBuffer;
	ID3D11Buffer*		indexBuffer;

	WorldBuffer*		worldBuffer;

	Shader*				shader;

	BOX_COLLIDER()
		: Center(0, 0, 0)
		, Size(1, 1, 1)
		, PointMax(0, 0, 0), PointMin(0, 0, 0)
		, color(135.0f / 255.0f, 208.0f / 255.0f, 128.0f / 255.0f, 1.0f)
		, worldBuffer(NULL)
		, vertexBuffer(NULL)
		, indexBuffer(NULL)
	{
		shader = new Shader(Shaders + L"BoxCollider.hlsl");

		worldBuffer = new WorldBuffer();
	}

	~BOX_COLLIDER()
	{
		SAFE_RELEASE(vertexBuffer);
		SAFE_RELEASE(indexBuffer);
		SAFE_DELETE(shader);
	}

	void CreateBuffer(void)
	{
		VertexColor vertex;
		vertex.color = color;
		vertexData.resize(8);
		vertex.position = D3DXVECTOR3(PointMin.x, PointMin.y, PointMin.z); vertexData[0] = vertex;
		vertex.position = D3DXVECTOR3(PointMin.x, PointMax.y, PointMin.z); vertexData[1] = vertex;
		vertex.position = D3DXVECTOR3(PointMax.x, PointMax.y, PointMin.z); vertexData[2] = vertex;
		vertex.position = D3DXVECTOR3(PointMax.x, PointMin.y, PointMin.z); vertexData[3] = vertex;
		vertex.position = D3DXVECTOR3(PointMin.x, PointMin.y, PointMax.z); vertexData[4] = vertex;
		vertex.position = D3DXVECTOR3(PointMin.x, PointMax.y, PointMax.z); vertexData[5] = vertex;
		vertex.position = D3DXVECTOR3(PointMax.x, PointMax.y, PointMax.z); vertexData[6] = vertex;
		vertex.position = D3DXVECTOR3(PointMax.x, PointMin.y, PointMax.z); vertexData[7] = vertex;

		/*
		 5----6
		1----2|
		||	 ||
		|4---|7
		0----3
		*/

		indexData.resize(24);
		indexData[0]  = 0;
		indexData[1]  = 1;
		indexData[2]  = 1;
		indexData[3]  = 2;
		indexData[4]  = 2;
		indexData[5]  = 3;
		indexData[6]  = 3;
		indexData[7]  = 0;

		indexData[8]  = 4;
		indexData[9]  = 5;
		indexData[10] = 5;
		indexData[11] = 6;
		indexData[12] = 6;
		indexData[13] = 7;
		indexData[14] = 7;
		indexData[15] = 4;
		
		indexData[16] = 0;
		indexData[17] = 4;

		indexData[18] = 1;
		indexData[19] = 5;

		indexData[20] = 2;
		indexData[21] = 6;

		indexData[22] = 3;
		indexData[23] = 7;


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

	void Update(D3DXMATRIX matWorld)
	{
		worldBuffer->SetMatrix(matWorld);
	}

	void Render(void)
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
	}

	void remakeVertex(void)
	{
		VertexColor vertex;
		vertexData[0].position = D3DXVECTOR3(Center.x + PointMin.x * Size.x, Center.y + PointMin.y * Size.y, Center.z + PointMin.z * Size.z);
		vertexData[1].position = D3DXVECTOR3(Center.x + PointMin.x * Size.x, Center.y + PointMax.y * Size.y, Center.z + PointMin.z * Size.z);
		vertexData[2].position = D3DXVECTOR3(Center.x + PointMax.x * Size.x, Center.y + PointMax.y * Size.y, Center.z + PointMin.z * Size.z);
		vertexData[3].position = D3DXVECTOR3(Center.x + PointMax.x * Size.x, Center.y + PointMin.y * Size.y, Center.z + PointMin.z * Size.z);
		vertexData[4].position = D3DXVECTOR3(Center.x + PointMin.x * Size.x, Center.y + PointMin.y * Size.y, Center.z + PointMax.z * Size.z);
		vertexData[5].position = D3DXVECTOR3(Center.x + PointMin.x * Size.x, Center.y + PointMax.y * Size.y, Center.z + PointMax.z * Size.z);
		vertexData[6].position = D3DXVECTOR3(Center.x + PointMax.x * Size.x, Center.y + PointMax.y * Size.y, Center.z + PointMax.z * Size.z);
		vertexData[7].position = D3DXVECTOR3(Center.x + PointMax.x * Size.x, Center.y + PointMin.y * Size.y, Center.z + PointMax.z * Size.z);

		gDC->UpdateSubresource(vertexBuffer, 0, NULL, &vertexData[0], sizeof(VertexColor) * vertexData.size(), 0);
	}

	bool MousePickked(D3DXVECTOR3 start, D3DXVECTOR3 direction, D3DXMATRIX world)
	{
		D3DXMATRIX invWorld;
		D3DXMatrixInverse(&invWorld, NULL, &world);

		D3DXVec3TransformCoord(&start, &start, &invWorld);
		D3DXVec3TransformNormal(&direction, &direction, &invWorld);

		UINT index[36] = {0, 1, 2, 0, 2, 3	 //   5----6
						, 7, 6, 5, 7, 5, 4	 //  /|	 /|
		                , 4, 5, 1, 4, 1, 0	 // 1----2 |
						, 3, 2, 6, 3, 6, 7	 // | |  | |
						, 1, 5, 6, 1, 6, 2	 // | 4--| 7
						, 4, 0, 3, 4, 3, 7}; // 0----3/

		for (UINT i = 0; i < 36; i += 3)
		{
			float u, v, distance;
			if (D3DXIntersectTri(&vertexData[index[i]].position,
								&vertexData[index[i + 1]].position,
								&vertexData[index[i + 2]].position,
								&start, &direction, &u, &v, &distance) == TRUE)
			{
				return true;
			}
		}
		return false;
	}

} BOX_COLLIDER;

#if 0
template <typename T>
class Component
{
public:
	Component() {}
	~Component() {}

	static void AddComponent(void) {}
	static void DeleteComponent(void) {}
	inline T GetComponent(void) { return data; }

private:
	T data;
};

class Component2
{
public:
	Component2() {}
	~Component2() {}
	
	template <typename T>
	static Component<T>* AddComponent(void) { return new Component<T>; }

private:

};
#endif