#include "stdafx.h"
#include "EnemyRespawnManager.h"
#include "../Executes/DrawStage.h"
#include "../Landscape/Terrain.h"
#include "../Objects/GModelReadWrite.h"
#include "../Units/GameEnemyRobot.h"

#define XBOT_MAX 1
#define YBOT_MAX 1

EnemyRespawnManager::EnemyRespawnManager(DrawStage* stage)
	: stage(stage)
	, battlePhase(EnemyRespawnPhase::Ready)
	, battleStart(false)
	, battleEnd(false)
{
	areas = stage->GetTerrain()->GetEnemyAreasRef();
}

EnemyRespawnManager::~EnemyRespawnManager()
{
}

void EnemyRespawnManager::Update(void)
{
	if (IsBattle() == false)
		return;

	if (areas.size() == 0)
		return;

	//enemy 개수 체크
	//-> 일정 수 이하일 경우 respawn 
	//-> 다음 Phase 로 이동
	switch (battlePhase)
	{
		case EnemyRespawnPhase::Ready:
		{
			EnemyArea area;
			FindArea("Phase1", &area);
			RespawnRobot(area, 1);
			battlePhase = EnemyRespawnPhase::Phase1;
		}
		break;
		case EnemyRespawnPhase::Phase1:
		{
		}
		break;
		case EnemyRespawnPhase::Phase2:
		{

		}
		break;
		case EnemyRespawnPhase::Boss:
		{

		}
		break;
	}
}

bool EnemyRespawnManager::FindArea(string name, OUT EnemyArea* area)
{
	size_t count = areas.size();
	for (size_t i = 0; i < count; i++)
	{
		if (areas[i].Name == name)
		{
			*area = areas[i];
			return true;
		}
	}

	assert(false);
	return false;
}

void EnemyRespawnManager::RespawnRobot(EnemyArea & area, UINT num)
{
	for (UINT i = 0; i < num; i++)
	{
		GameEnemyRobot* robot = NULL;

		//Random Robot
		wstring gmodelFile = L"";
		{
			if (Math::Random(0, 1) == 0)
			{
				//XBot
				int num = Math::Random(1, XBOT_MAX);
			}
			else
			{
				//YBot

			}
		}





		GModelReadWriter::OpenGModel(gmodelFile, GModelReadWriter::GModelReadClass::GameEnemyRobot, (void**)&robot);
		robot->LinkAddress(stage->GetPlayer());
		stage->AddEnemy(robot);
	}
}
