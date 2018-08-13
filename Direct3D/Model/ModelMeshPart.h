#pragma once
#include "../Interfaces/ICloneable.h"

class Model;
class ModelMesh;

class ModelMeshPart : public ICloneable
{
public:
	friend Model;
	friend class Models;
	friend ModelMesh;

private:
	ModelMeshPart();
	~ModelMeshPart();
	// ICloneable을(를) 통해 상속됨
	virtual void Clone(void ** clone) override;


	void Binding(void);
	void Render(void);

	void ScanPointMinMax(D3DXVECTOR3 * min, D3DXVECTOR3 * max, D3DXMATRIX& matWorld);


private:
	ModelMesh*				parent;

	wstring					materialName;
	Material*				material;

	vector<ModelVertexType> vertices;
	vector<UINT>			indices;

	//클래스 내에서 생성하는 변수들
	ID3D11Buffer*			vertexBuffer;
	ID3D11Buffer*			indexBuffer;



};
