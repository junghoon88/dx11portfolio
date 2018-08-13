#pragma once
#include "../Interfaces/ICloneable.h"

class ModelMeshPart;
class ModelBone;

class ModelMesh : public ICloneable
{
public:
	friend class Model;
	friend class Models;

private:
	ModelMesh();
	~ModelMesh();
	// ICloneable을(를) 통해 상속됨
	virtual void Clone(void** clone) override;

	
	void Binding(void);

public:
	void Render(void);

	void ScanPointMinMax(D3DXVECTOR3 * min, D3DXVECTOR3 * max);

public:
	inline wstring GetName(void)			{ return name; }
	inline void SetWorld(D3DXMATRIX& world) { worldBuffer->SetMatrix(world); }
	inline int GetParentBoneIndex(void)		{ return parentBoneIndex; }
	inline ModelBone* GetParentBone(void)	{ return parentBone; }

private:
	wstring name;

	int parentBoneIndex;
	ModelBone* parentBone;

	WorldBuffer* worldBuffer;
	vector<ModelMeshPart*> meshParts;


};
