#pragma once
#include "stdafx.h"

struct EnemyMoveRoute
{
	map<float, D3DXVECTOR3> Routes;

	D3DXVECTOR3 GetRoute(float time)
	{
		D3DXVECTOR3 pos(0, 0, 0);

		if (Catmullrom(time, pos))
		{
			return pos;
		}
		else
		{

		}

		return pos;
	}

private:
	bool Catmullrom(float time, D3DXVECTOR3& pos)
	{
		if (Routes.size() < 4)
			return false;

		map<float, D3DXVECTOR3>::iterator iter1 = Routes.begin();
		map<float, D3DXVECTOR3>::iterator iter2 = iter1;
		iter2++;

		while (iter2 != Routes.end())
		{
			if (time >= iter1->first && time <= iter2->first)
			{
				float dt = iter2->first - iter1->first;
				time = time - iter1->first;
				assert(dt > EPSILON6);
				float s = time / dt;

				D3DXVECTOR3 V0, V1, V2, V3;
				V1 = iter1->second;
				V2 = iter2->second;
				
				if (iter1 == Routes.begin()) 
					iter1 = Routes.end();
				iter1--;
				V0 = iter1->second;

				iter2++;
				if (iter2 == Routes.end())
					iter2 = Routes.begin();
				V3 = iter2->second;

				D3DXVec3CatmullRom(&pos, &V0, &V1, &V2, &V3, s);
				return true;
			}
			else
			{
				iter1++;
				iter2++;
			}
		}

		return false;
	}
};
