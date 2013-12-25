#pragma once

#include "FpsUtil.h"
#include <vector>

class GameTimer
{
public:
	GameTimer();
	virtual ~GameTimer();
	void Init();
	float Frame();
	void CalculateFps();
	inline float GetRunTime() {return this->zRunTime;}
	inline float GetDeltaTime() {return this->zDeltaTime;}
	inline int GetFPS() {return this->zFramesPerSec;}
	
	FpsUtil* GetFpsUtility() {return this->zFpsUtility;}
private:
	float zSecsPerCnt;
	INT64 zStartTime;
	float zDeltaTime;
	float zRunTime;
	int zFramesPerSec;
	int zFrameCounter;
	std::vector<float> zDt;

	//Fps
	FpsUtil* zFpsUtility;

};