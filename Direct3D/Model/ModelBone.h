#pragma once
#include "../Interfaces/ICloneable.h"

class ModelBone : public ICloneable
{
public:
	friend class Model;
	friend class Models;

private:
	ModelBone();
	~ModelBone();

public:
	// ICloneable을(를) 통해 상속됨
	virtual void Clone(void ** clone) override;

public:
	inline int GetIndex(void) { return index; }
	inline wstring GetName(void) { return name; }
	inline int GetParentIndex(void) { return parentIndex; }

	inline ModelBone* GetParent(void) { return parent; }

	inline void SetTransform(D3DXMATRIX& transform) { this->transform = transform; }
	inline D3DXMATRIX& GetTransform(void) { return transform; }
	inline void SetAbsTransform(D3DXMATRIX& transform) { this->absTransform = transform; }
	inline D3DXMATRIX& GetAbsTransform(void) { return absTransform; }

	inline void AddChild(ModelBone* child) { childs.push_back(child); }

	inline UINT GetChildCount(void) { return childs.size(); }
	inline ModelBone* GetChild(UINT index) { return childs[index]; }

private:
	int   index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	vector<ModelBone *> childs;

	D3DXMATRIX transform;
	D3DXMATRIX absTransform;

};