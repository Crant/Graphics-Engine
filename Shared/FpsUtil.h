#pragma once

#include <windows.h>

class FpsUtil
{
private:
	//Fps Limit
	int zMinFps;
	int zMaxFps;
	float zTargetLowerDelta;
	float zTargetUpperDelta;

	//Buffers used to store deltaTime
	float zLowerLimitAcc;
	float zUpperLimitAcc;

public:
	FpsUtil();
	virtual ~FpsUtil(){}

	void Update(float& deltaTime);
	//Sleeps the remaining time to limit fps
	void SleepProgram(float& deltaTime);
	void Limit(float& deltaTime);

	void SetFPSUpperLimit(unsigned int fps);
	void SetFPSLowerLimit(unsigned int fps);
};