#pragma once

#define gTime Time::Get()

class Time
{
private:
	Time();
	~Time();

public:
	inline static Time* Get(void)   { assert(instance != NULL); return instance; }
	inline static void Create(void) { assert(instance == NULL); instance = new Time(); }
	inline static void Delete(void) { SAFE_DELETE(instance); }

	inline static bool Stopped(void) { return isTimerStopped; }
	inline static float Delta(void) { return isTimerStopped ? 0.0f : timeElapsed; }

	void Update(UINT lockFPS = 60);
	void Print(void);

	void Start(void);
	void Stop(void);

	inline UINT GetFPS(void) const { return FPS; }
	inline float RunningTime(void) const { return runningTime; }

private:
	static Time* instance;
	static bool isTimerStopped;
	static float timeElapsed;

	float timeScale;
	INT64 currentCount;
	INT64 lastCount;
	float FPSTimeElpased;

	UINT frameCount;
	UINT FPS;
	float runningTime;
};

