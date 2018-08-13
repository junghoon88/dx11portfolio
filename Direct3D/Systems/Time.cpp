#include "stdafx.h"
#include "Time.h"

Time* Time::instance = NULL;
bool  Time::isTimerStopped = true;
float Time::timeElapsed = 0.0f;


Time::Time()
	: currentCount(0)
	, lastCount(0)
	, FPSTimeElpased(0.0f)
	, frameCount(0)
	, runningTime(0.0f)
{
	//XP 이상부터 지원됨
	INT64 frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

	assert(frequency != 0);

	timeScale = 1.0f / frequency;
}


Time::~Time()
{
}


void Time::Update(UINT lockFPS)
{
	assert(lockFPS != 0);

	if (isTimerStopped) return;

	//1. 현재시간을 가져와 시간 간격 및 진행시간을 계산한다.
	QueryPerformanceCounter((LARGE_INTEGER*)&currentCount);
	timeElapsed = (float)(currentCount - lastCount) * timeScale;

	while (timeElapsed < 1.0f / (float)lockFPS)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&currentCount);
		timeElapsed = (float)(currentCount - lastCount) * timeScale;
	}
	runningTime += timeElapsed;
	FPSTimeElpased += timeElapsed;

	//2. FPS Update
	frameCount++;

	if (FPSTimeElpased >= 1.0f)
	{
		FPS = frameCount;
		//cout << "FPS : " << FPS << ", timeElapsed : " << timeElapsed << endl;
		frameCount = 0;
		FPSTimeElpased -= 1.0f;
	}

	lastCount = currentCount;
}

void Time::Print(void)
{
}

void Time::Start(void)
{
	assert(isTimerStopped);

	QueryPerformanceCounter((LARGE_INTEGER*)&lastCount);
	isTimerStopped = false;
}

void Time::Stop(void)
{
	assert(!isTimerStopped);

	INT64 stopCount = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&stopCount);
	runningTime += (float)(stopCount - lastCount) * timeScale;
	isTimerStopped = true;
}
