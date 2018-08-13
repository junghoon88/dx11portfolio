#pragma once

enum class SellAttribute;

struct Tile
{
	int idX;
	int idY;

	SellAttribute Attribute;
	float AvgHeight;
	bool isLanded;


	//F = G + H
	// F == �� ��� 
	// G == ���������κ��� ���� ���(Ÿ��) ������ ��κ��
	// H == ���� ���(Ÿ��)���� ������������ ��κ��

	float CostTotal;		//F
	float CostFromStart;	//G
	float CostToGoal;		//H

	bool IsOpen;

	Tile* ParentNode;

	Tile()
		: CostTotal(0), CostFromStart(0), CostToGoal(0)
		, IsOpen(true), ParentNode(NULL)
	{
	}
};

typedef vector<Tile*> vTile;
typedef vector<Tile*>::iterator vTileIter;

namespace Landscape { class Terrain; }

class AStar
{
public:
	//Thread
	void RequestPathFind(Model::BinModel* model, D3DXVECTOR2 start, D3DXVECTOR2 end, float* movableHeight);
private:
	void PathFindProcForModel(Model::BinModel* model, D3DXVECTOR2 start, D3DXVECTOR2 end, float movableHeight);
	void MornitorThread(void);

public:
	AStar(Landscape::Terrain* terrain);
	~AStar();


	//Ÿ�� ������ �Լ�
	void ClearTiles(void);
	void SetTiles(D3DXVECTOR2 startVec, D3DXVECTOR2 endVec);
	void UpdateMapInfo(void);


	//���� Ÿ�Ͽ��� ���� �ִ� �� ã�� �Լ�
	void AddOpenList(Tile* currentTile);

	void PathFinder(void);


private:
	void Set(void);
	void Delete(void);

	//��� Ž�� �Լ�
	void PathFinder(Tile* currentTile);


public:
	inline vTile GetTotalList(void) { return vTotalList; }
	inline vTile GetCloseList(void) { return vCloseList; }

	inline Tile* GetStartTile(void) { return staTile; }

	inline UINT GetWidth(void) { return width; }

	//inline void SetMovableHeight(float height) { movableHeight = height; }
	
	inline bool GetThreadExit(void) { return threadExit; }


private:
	vTile vTotalList;	// ��Ż Ÿ��
	vTile vOpenList;	// �� �� �ִ� Ÿ��
	vTile vCloseList;	// ���� ����� Ÿ��

	Tile* staTile;		// ���� Ÿ��
	Tile* endTile;		// ��   Ÿ��
	Tile* curTile;		// ���� Ÿ��

	bool  valid;

	int findCnt;
	int findCntMax;

	//�ͷ��� ����
	Landscape::Terrain* terrain;
	UINT width, height;

	//������ �Ѿ�� �ִ� �ִ� ����
	float movableHeight;

	//Thread
	bool			threadExit;
	thread*			mornitorThread;
	vector<thread*> vecThread;
	mutex			mtx;
};

