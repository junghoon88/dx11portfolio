#pragma once

class BoundingBox;
class BoundingFrustum;
class BoundingSphere;
class BoundingCapsule;

#define gModelShape ModelShape::Get()

class DebugShape
{
public:
	DebugShape(UINT vertexCount, UINT lineCount)
	{
		Vertices.resize(vertexCount);
		LineCount = lineCount;
	}
	vector<VertexColor> Vertices;
	UINT				LineCount;
};

class ModelShape
{
public:
	inline static ModelShape* Get(void) { return instance; }
	inline static void Create(void) { instance = new ModelShape(); }
	inline static void Delete(void) { SAFE_DELETE(instance); }

private:
	ModelShape();
	~ModelShape();
	static ModelShape* instance;

public:
	void PreRender(void);
	void Render(void);
	void PostRender(void);

	void ResizeScreen(void);

	void ClearBuffer(void);
	void CreateBuffer(void);

	void SetDragRect(RECT rcDrag, D3DXCOLOR* color = NULL);

public:
	void AddLine(D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXCOLOR color);
	void AddTriangle(D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c, D3DXCOLOR color);
	void AddBoundingFrustum(BoundingFrustum* frustum, D3DXCOLOR color);
	void AddBoundingBox(BoundingBox* box, D3DXCOLOR color);
	void AddBoundingSphere(BoundingSphere* sphere, D3DXCOLOR color);
	//void AddBoundingCylinder(BoundingCylinder* cylinder, D3DXCOLOR color);
	void AddBoundingCapsule(BoundingCapsule* cylinder, D3DXCOLOR color);

private:
	void InitializeDragRect(void);
	DebugShape* GetShapeForLines(int lineCount);


private:
	vector<DebugShape*> activeShapes;


	//shader buffer
	vector<VertexColor> vertexData;
	vector<UINT>		indexData;

	ID3D11Buffer*		vertexBuffer;
	ID3D11Buffer*		indexBuffer;

	WorldBuffer*		worldBuffer;

	Shader*				shader;


	//drag
	bool				isDraging;
	VertexColor			vertexData2[4];
	UINT				indexData2[8];
	ID3D11Buffer*		vertexBuffer2;
	ID3D11Buffer*		indexBuffer2;
	ID3D11BlendState*	blendState[2];
	Shader*				shader2;

};



