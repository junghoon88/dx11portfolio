#pragma once
#include "stdafx.h"

struct GameDataSpec
{
};

struct GamePlayerSpec : public GameDataSpec
{
	//1레벨 기준 스펙
	float HP1Lv  = 100.0f;
	float MP1Lv  = 100.0f;
	float Exp1Lv = 100.0f;
	float Atk1Lv = 10.0f;
	float Dep1Lv = 5.0f;

	//레벨당 추가 수치
	float HPpLv  = 20.0f;
	float MPpLv  = 10.0f;
	float ExppLv = 20.0f;
	float AtkpLv = 3.0f;
	float DeppLv = 1.0f;

	//
	float RunSpeed          = 20.0f;
	float WalkSpeed         = 10.0f;
	float WalkBackwardSpeed = 10.0f;


};

struct GameEnemySpec : public GameDataSpec
{
	float MoveSpeed = 4.0f;
	float FollowRange = 20.0f;
	float AttackRange = 5.0f;
	float AttackDelay = 2.0f;
};

struct GameEnemyRobotSpec : public GameEnemySpec
{
	//float MoveSpeed = 4.0f;
	//float FollowRange = 20.0f;
	//float AttackRange = 5.0f;
};

