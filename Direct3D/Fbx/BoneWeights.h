#pragma once
#include "stdafx.h"

struct MyFbxBlendWeight
{
	D3DXVECTOR4 Indices = D3DXVECTOR4(0, 0, 0, 0);
	D3DXVECTOR4 Weights  = D3DXVECTOR4(0, 0, 0, 0);

	void Set(int index, int boneIndex, float weight)
	{
		assert(index >= 0 && index < 4);
		Indices[index] = (float)boneIndex;
		Weights[index] = weight;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
struct MyFbxBoneWeights
{
private:
	typedef pair<int, float> Pair;
	vector<Pair> BoneWeights;

public:
	void AddBoneWeight(int boneIndex, float boneWeight)
	{
		if (boneWeight <= 0.0f)
			return;

		bool bAdd = false;

		auto iter = BoneWeights.begin();
		for (; iter != BoneWeights.end(); ++iter)
		{
			if (boneWeight > iter->second)
			{
				//입력값이 현재 벡터 이터레이터 값보다 크면 바로앞에 삽입
				BoneWeights.insert(iter, Pair(boneIndex, boneWeight));
				bAdd = true;

				break;
			}
		}

		if (bAdd == false && BoneWeights.size() < 4)
			BoneWeights.push_back(Pair(boneIndex, boneWeight));
	}

	void AddBoneWeights(MyFbxBoneWeights& boneWeights)
	{
		vector<Pair>& temp = boneWeights.BoneWeights;
		for (size_t i = 0; i < temp.size(); i++)
		{
			AddBoneWeight(temp[i].first, temp[i].second);
		}
	}

	void GetBlendWeights(MyFbxBlendWeight& blendWeights)
	{
		int i = 0;
		auto iter = BoneWeights.begin();
		while (iter != BoneWeights.end())
		{
			blendWeights.Set(i, iter->first, iter->second);
			i++, iter++;

			if (i >= 4)
				break;
		}
	}

	void Normalize(void)
	{
		if (BoneWeights.size() == 0)
			return;

		float totalWeight = 0.0f;

		int i = 0; 
		auto iter = BoneWeights.begin();
		while (iter != BoneWeights.end())
		{
			if (i < 4)
			{
				totalWeight += iter->second;
				++i;
				++iter;
			}
			else
			{
				//4개보다 많으면 삭제
				iter = BoneWeights.erase(iter);
			}
		}

		float scale = 1.0f / totalWeight;
		iter = BoneWeights.begin();
		while (iter != BoneWeights.end())
		{
			iter->second *= scale;
			++iter;
		}
	}
};