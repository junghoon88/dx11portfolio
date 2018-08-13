#include "stdafx.h"
#include "ModelAnimClip.h"

ModelAnimClip::ModelAnimClip()
	: defaltFrameRate(0.0), loop(true), endless(false)
{
}

ModelAnimClip::~ModelAnimClip()
{
	for (pair<wstring, ModelKeyframe *> temp : keyframeMap)
		SAFE_DELETE(temp.second);
}

void ModelAnimClip::Clone(void ** clone)
{
	ModelAnimClip* clip = new ModelAnimClip();
	clip->filename = filename;
	clip->name = name;

	clip->totalFrame = totalFrame;
	clip->frameRate = frameRate;
	clip->defaltFrameRate = defaltFrameRate;

	for (auto& key : keyframeMap)
	{
		ModelKeyframe* temp = NULL;
		key.second->Clone((void **)&temp);

		clip->keyframeMap[temp->BoneName] = temp;
	}

	*clone = clip;
}