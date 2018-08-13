#pragma once
#include "../Interfaces/ICloneable.h"

struct ModelKeyframeData
{
	D3DXMATRIX Transform;
	D3DXQUATERNION Rotation;
	D3DXVECTOR3 Translation;
	D3DXVECTOR3 Scale;
};

struct ModelKeyframe : public ICloneable
{
	wstring BoneName;
	vector<ModelKeyframeData> Datas;

	void Clone(void** clone)
	{
		ModelKeyframe* frame = new ModelKeyframe();

		frame->BoneName = BoneName;
		for (ModelKeyframeData temp : Datas)
		{
			ModelKeyframeData data;
			data.Transform = temp.Transform;
			data.Rotation = temp.Rotation;
			data.Translation = temp.Translation;
			data.Scale = temp.Scale;

			frame->Datas.push_back(data);
		}

		*clone = frame;
	}
};

class ModelAnimClip : public ICloneable
{
public:
	friend class Model;
	friend class Models;

public:
	void Clone(void** clone);

	inline wstring GetFileName(void) { return filename; }
	inline wstring GetName(void) { return name; }
	inline int GetTotalFrame(void) { return totalFrame; }
	inline float GetFrameRate(void) { return frameRate; }
	inline ModelKeyframe* GetKeyframe(wstring name) { return keyframeMap[name]; }

	inline bool GetLoop(void) { return loop; }
	inline void SetLoop(bool value) { loop = value; }
	inline bool GetEndless(void) { return endless; }
	inline void SetEndless(bool value) { endless = value; }

private:
	ModelAnimClip();
	~ModelAnimClip();

	wstring filename;
	wstring name;

	int totalFrame;
	float frameRate;
	float defaltFrameRate;
	bool loop;
	bool endless;

	unordered_map<wstring, ModelKeyframe*> keyframeMap;
};