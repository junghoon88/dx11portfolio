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
	// ICloneable��(��) ���� ��ӵ�
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

	//Ŭ���� ������ �����ϴ� ������
	ID3D11Buffer*			vertexBuffer;
	ID3D11Buffer*			indexBuffer;



};
