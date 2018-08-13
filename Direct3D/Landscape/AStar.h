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
	// F == 총 비용 
	// G == 시작점으로부터 현재 노드(타일) 까지의 경로비용
	// H == 현재 노드(타일)부터 도착점까지의 경로비용

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


	//타일 셋팅할 함수
	void ClearTiles(void);
	void SetTiles(D3DXVECTOR2 startVec, D3DXVECTOR2 endVec);
	void UpdateMapInfo(void);


	//현재 타일에서 갈수 있는 길 찾는 함수
	void AddOpenList(Tile* currentTile);

	void PathFinder(void);


private:
	void Set(void);
	void Delete(void);

	//경로 탐색 함수
	void PathFinder(Tile* currentTile);


public:
	inline vTile GetTotalList(void) { return vTotalList; }
	inline vTile GetCloseList(void) { return vCloseList; }

	inline Tile* GetStartTile(void) { return staTile; }

	inline UINT GetWidth(void) { return width; }

	//inline void SetMovableHeight(float height) { movableHeight = height; }
	
	inline bool GetThreadExit(void) { return threadExit; }


private:
	vTile vTotalList;	// 토탈 타일
	vTile vOpenList;	// 갈 수 있는 타일
	vTile vCloseList;	// 가장 가까운 타일

	Tile* staTile;		// 시작 타일
	Tile* endTile;		// 끝   타일
	Tile* curTile;		// 현재 타일

	bool  valid;

	int findCnt;
	int findCntMax;

	//터레인 정보
	Landscape::Terrain* terrain;
	UINT width, height;

	//유닛이 넘어갈수 있는 최대 높이
	float movableHeight;

	//Thread
	bool			threadExit;
	thread*			mornitorThread;
	vector<thread*> vecThread;
	mutex			mtx;
};

