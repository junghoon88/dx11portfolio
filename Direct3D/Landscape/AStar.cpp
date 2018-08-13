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
			//thread ���� ���
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

	//������ġ�� ó��������ġ�� ��������
	//curTile = staTile;
	endTile = vTotalList[UINT(endPt.x + endPt.y * width)];

	//endTile �� �ٸ� ��ü�� ���� ��� start �������� �ű��.
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

	//TODO : �ͷ������� �ٲ�� ����
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

//���� Ÿ�Ͽ��� ���� �ִ� �� ã�� �Լ�
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

			//�������� ����
			{
				float curAvgHeight = currentTile->AvgHeight;
				float nextAvgHeight = node->AvgHeight;

				//���̰� �ʹ� ���ų� �ʹ� ������ �������� �Ѵ�.
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


//��� Ž�� �Լ�
void AStar::PathFinder(Tile* currentTile)
{
	if (!valid)
		return;


	float tempTotalCost = 5000;
	Tile* tempTile = NULL;

	AddOpenList(currentTile);

	for (UINT i = 0; i < vOpenList.size(); i++)
	{
		//cost ����
		//�ε��� Ÿ�Ϻ��� endŸ�ϱ��� ���� �Ÿ�
		vOpenList[i]->CostToGoal = 
			float((abs(endTile->idX - vOpenList[i]->idX) + abs(endTile->idY - vOpenList[i]->idY)) * 10);

		//�ε��� Ÿ���� �θ�κ��� �ε��� Ÿ�ϱ��� 
		{
			Tile* s = vOpenList[i];
			Tile* p = vOpenList[i]->ParentNode;

			//�⺻��
			vOpenList[i]->CostFromStart = 10;

			//�������� ����
			float deltaAvgHeight = s->AvgHeight - p->AvgHeight; //�������� ���� ���̰� ������ +

			vOpenList[i]->CostFromStart += (deltaAvgHeight / movableHeight) * 10.0f;

			//�밢������ ����
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

		//�ڽ�Ʈ�߿��� min ���� tempTile �� ����
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

	//���� �Ͽ� ������ġ�� �����ϰ� �˻��ؾ� �����.(������ �����濡�� ��� ���� ��ġ�� �ӹ���)
	for (auto iter = vOpenList.begin(); iter != vOpenList.end(); ++iter)
	{
		if (*iter == tempTile)
		{
			iter = vOpenList.erase(iter);
			break;
		}
	}

	curTile = tempTile;

	//����Լ�
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

	vTotalList.clear();	// ��Ż Ÿ��
	vOpenList.clear();	// �� �� �ִ� Ÿ��
	vCloseList.clear();	// ���� ����� Ÿ��
}
