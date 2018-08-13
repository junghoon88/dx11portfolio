#include "stdafx.h"
#include "AStar.h"
#include "Terrain.h"
#include "TerrainData.h"

#include "../Utilities/Math.h"

using namespace Landscape;

//Thread
void AStar::RequestPathFind(Model::BinModel * model, D3DXVECTOR2 start, D3DXVECTOR2 end, float* movableHeight)
{
	float height = this->movableHeight;
	if (movableHeight)
		height = *movableHeight;

	thread* th = new thread(&AStar::PathFindProcForModel, this, model, start, end, height);
	vecThread.push_back(th);

	model->SetAstarBusy(true);
}

void AStar::PathFindProcForModel(Model::BinModel * model, D3DXVECTOR2 start, D3DXVECTOR2 end, float movableHeight)
{
	mtx.lock();

	UpdateMapInfo();
	ClearTiles();
	SetTiles(start, end);

	this->movableHeight = movableHeight;

	PathFinder();
	model->SetMoveTarget(vCloseList);

	model->SetAstarBusy(false);

	mtx.unlock();
}

void AStar::MornitorThread(void)
{
	while (true)
	{
		for (UINT i = 0; i < vecThread.size();)
		{
			//thread 종료 대기
			if (vecThread[i]->joinable())
			{
				vecThread[i]->join();
				vecThread.erase(vecThread.begin() + i);
			}
			else ++i;
		}

		if (vecThread.size() == 0 && threadExit == true)
		{
			break;
		}

		Sleep(100);
	}
}
//~Thread


AStar::AStar(Terrain* terrain)
	: terrain(terrain)
	, staTile(NULL), endTile(NULL), curTile(NULL)
	, valid(false), width(0), height(0)
	, findCnt(0), findCntMax(100)
	, movableHeight(5.0f)
	, threadExit(false)
{
	Set();

	mornitorThread = new thread(&AStar::MornitorThread, this);
}

AStar::~AStar()
{
	threadExit = true;
	mornitorThread->join();


	Delete();
}

void AStar::ClearTiles(void)
{
	vCloseList.clear();
	vOpenList.clear();

	for (UINT i = 0; i < vTotalList.size(); i++)
	{
		vTotalList[i]->ParentNode = NULL;
		vTotalList[i]->IsOpen = true;
	}

	findCnt = 0;
}

void AStar::SetTiles(D3DXVECTOR2 startVec, D3DXVECTOR2 endVec)
{
	POINT startPt, endPt;
	startPt.x = (LONG)startVec.x;
	startPt.y = (LONG)startVec.y;
	endPt.x   = (LONG)endVec.x;
	endPt.y   = (LONG)endVec.y;


	if ((startPt.x < 0 || startPt.x >= width || startPt.y < 0 || startPt.y >= height)
		|| (endPt.x < 0 || endPt.x >= width || endPt.y < 0 || endPt.y >= height))
	{
		valid = false;
		return;
	}


	if (startPt.x == endPt.x && startPt.y == endPt.y)
	{
		valid = false;
		return;
	}

	//startTile
	staTile = vTotalList[UINT(startPt.x + startPt.y * width)];

	//현재위치는 처음시작위치와 같도록함
	//curTile = staTile;
	endTile = vTotalList[UINT(endPt.x + endPt.y * width)];

	//endTile 에 다른 객체가 있을 경우 start 방향으로 옮긴다.
	if (endTile->isLanded || endTile->Attribute == SellAttribute::MOVE_DISABLE)
	{
		POINT dir = { 0, 0 };
		if (startPt.x < endPt.x) dir.x = -1;
		if (startPt.x > endPt.x) dir.x =  1;

		if (startPt.y < endPt.y) dir.y = -1;
		if (startPt.y > endPt.y) dir.y = 1;

		int absDx = abs(startPt.x - endPt.x);
		int absDy = abs(startPt.y - endPt.y);

		float distMin = Math::FloatMaxValue;
		
		Tile* nextEndTile = NULL;
		POINT tempPt = endPt;

		for (int x = 0; x < absDx; x++)
		{
			tempPt.y = endPt.y;
			for (int y = 0; y < absDy; y++)
			{
				if (x == 0 && y == 0)
					continue; 

				tempPt.y += dir.y;

				Tile* tempTile = vTotalList[UINT(tempPt.x + tempPt.y * width)];

				if (tempTile->isLanded || tempTile->Attribute == SellAttribute::MOVE_DISABLE)
					continue;

				float dist = pow(startPt.x - tempPt.x, 2) + pow(startPt.y - tempPt.y, 2);

				if (dist < distMin)
				{
					distMin = dist;
					nextEndTile = tempTile;
				}
			}
			tempPt.x += dir.x;
		}

		if (nextEndTile == NULL)
		{
			valid = false;
			return;
		}
	}

	valid = true;
}

void AStar::UpdateMapInfo(void)
{
	UINT widthOld = width;
	UINT heightOld = height;
	terrain->GetTerrainData()->GetTerrainSize(&width, &height);
	assert(width*height != 0);

	//TODO : 터레인정보 바뀌면 수정
	//if (width != widthOld || height != heightOld)
	//{
	//	Delete();
	//}

	//UINT gridSpacing = terrain->GetGridSpacing();

	Sell* sells = terrain->GetTerrainData()->GetAstarSells();
	assert(sells != NULL);

	for (UINT i = 0; i < width * height; i++)
	{
		vTotalList[i]->AvgHeight = sells[i].AvgHeight;
		vTotalList[i]->Attribute = sells[i].Attribute;
		vTotalList[i]->isLanded  = sells[i].isLanded;
	}
}

//현재 타일에서 갈수 있는 길 찾는 함수
void AStar::AddOpenList(Tile* currentTile)
{
	//curTile = currentTile;

	int startX = currentTile->idX - 1;
	int startY = currentTile->idY - 1;

	for (int i = 0; i < 3; i++) //y
	{
		for (int j = 0; j < 3; j++) //x
		{
			if (i == 1 && j == 1) continue;
			if ((startX + j) < 0) continue;
			if ((startY + i) < 0) continue;
			if ((startX + j) > (int)width)  continue;
			if ((startY + i) > (int)height) continue;


			Tile* node = vTotalList[((startY + i) * width) + (startX + j)];

			if (!node->IsOpen) continue;
			if (node == staTile) continue;
			if (node->Attribute == SellAttribute::MOVE_DISABLE) continue;

			//높이차이 기준
			{
				float curAvgHeight = currentTile->AvgHeight;
				float nextAvgHeight = node->AvgHeight;

				//높이가 너무 높거나 너무 낮으면 못가도록 한다.
				if (abs(nextAvgHeight - curAvgHeight) > movableHeight)
					continue;
			}

			node->ParentNode = currentTile;

			bool addObj = true;

			for (vTileIter _viOpenList = vOpenList.begin(); _viOpenList != vOpenList.end(); ++_viOpenList)
			{
				if (*_viOpenList == node)
				{
					addObj = false;
					break;
				}
			}

			if (node != endTile)
			{
				//node->setColor(RGB(128, 64, 28));
			}

			if (!addObj) continue;

			vOpenList.push_back(node);
		}
	}

	if (vOpenList.size() == 0)
	{
		printf("");
	}
}

void AStar::PathFinder(void)
{
	PathFinder(staTile);
}


//경로 탐색 함수
void AStar::PathFinder(Tile* currentTile)
{
	if (!valid)
		return;


	float tempTotalCost = 5000;
	Tile* tempTile = NULL;

	AddOpenList(currentTile);

	for (UINT i = 0; i < vOpenList.size(); i++)
	{
		//cost 연산
		//인덱스 타일부터 end타일까지 남은 거리
		vOpenList[i]->CostToGoal = 
			float((abs(endTile->idX - vOpenList[i]->idX) + abs(endTile->idY - vOpenList[i]->idY)) * 10);

		//인덱스 타일의 부모로부터 인덱스 타일까지 
		{
			Tile* s = vOpenList[i];
			Tile* p = vOpenList[i]->ParentNode;

			//기본값
			vOpenList[i]->CostFromStart = 10;

			//높이차이 기준
			float deltaAvgHeight = s->AvgHeight - p->AvgHeight; //이전보다 현재 높이가 높으면 +

			vOpenList[i]->CostFromStart += (deltaAvgHeight / movableHeight) * 10.0f;

			//대각선인지 여부
			int len = 0;
			len += abs(s->idX - p->idX);
			len += abs(s->idY - p->idY);

			if (len == 1)
				vOpenList[i]->CostFromStart *= 1.0f;
			else if (len == 2)
				vOpenList[i]->CostFromStart *= 1.4f;
			else
				assert(false);
		}

		//
		vOpenList[i]->CostTotal = vOpenList[i]->CostToGoal + vOpenList[i]->CostFromStart;

		//코스트중에서 min 값아 tempTile 에 보관
		if (tempTotalCost > vOpenList[i]->CostTotal)
		{
			tempTotalCost = vOpenList[i]->CostTotal;
			tempTile = vOpenList[i];
		}

		vOpenList[i]->IsOpen = false;
	}

	assert(tempTile != NULL);

	findCnt++;
	if (findCnt > findCntMax)
	{
		while (tempTile->ParentNode != NULL)
		{
			vCloseList.push_back(tempTile);
			tempTile = tempTile->ParentNode;
		}

		return;
	}

	if (tempTile == endTile)
	{
		while (tempTile->ParentNode != NULL)
		{
			vCloseList.push_back(tempTile);
			tempTile = tempTile->ParentNode;
		}

		return;
	}

	//다음 턴에 현재위치를 제외하고 검색해야 진행됨.(없으면 막힌길에서 계속 같은 위치로 머무름)
	for (auto iter = vOpenList.begin(); iter != vOpenList.end(); ++iter)
	{
		if (*iter == tempTile)
		{
			iter = vOpenList.erase(iter);
			break;
		}
	}

	curTile = tempTile;

	//재귀함수
	PathFinder(curTile);
}





void AStar::Set(void)
{
	assert(terrain != NULL);

	terrain->GetTerrainData()->GetTerrainSize(&width, &height);
	assert(width*height != 0);

	//UINT gridSpacing = terrain->GetGridSpacing();

	Sell* sells = terrain->GetTerrainData()->GetAstarSells();
	assert(sells != NULL);

	for (UINT i = 0; i < width * height; i++)
	{
		Tile* node = new Tile;
		node->idX = i % width;
		node->idY = i / width;

		node->AvgHeight = sells[i].AvgHeight;
		node->Attribute = sells[i].Attribute;
		node->isLanded  = sells[i].isLanded;
		vTotalList.push_back(node);
	}
}

void AStar::Delete(void)
{
	for (UINT i = 0; i < vTotalList.size();)
	{
		if (vTotalList[i] != NULL)
		{
			SAFE_DELETE(vTotalList[i]);
			vTotalList.erase(vTotalList.begin() + i);
		}
		else ++i;
	}

	vTotalList.clear();	// 토탈 타일
	vOpenList.clear();	// 갈 수 있는 타일
	vCloseList.clear();	// 가장 가까운 타일
}
