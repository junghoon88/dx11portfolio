#include "stdafx.h"
#include "ModelBone.h"

ModelBone::ModelBone()
	: parent(NULL)
{
}

ModelBone::~ModelBone()
{
}

void ModelBone::Clone(void ** clone)
{
	ModelBone* bone = new ModelBone();

	bone->index = this->index;
	bone->name = this->name;
	bone->parentIndex = this->parentIndex;
	//ModelBone* parent;
	//vector<ModelBone *> childs;
	bone->transform = this->transform;
	bone->absTransform = this->absTransform;

	*clone = bone;
}
