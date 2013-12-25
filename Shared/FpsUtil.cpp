#include "FpsUtil.h"
#include <math.h>

FpsUtil::FpsUtil()
{
	this->zMaxFps = 0;
	this->zMinFps = 0;
	this->zTargetLowerDelta = 0.0f;
	this->zTargetUpperDelta = 0.0f;

	this->zLowerLimitAcc = 0.0f;
	this->zUpperLimitAcc = 0.0f;
}

void FpsUtil::SetFPSUpperLimit(unsigned int fps )
{
	if (this->zMinFps != 0 && fps > this->zMinFps)
	{
		this->zMaxFps = fps;
		this->zTargetUpperDelta = 1.0f / this->zMaxFps;
	}
}

void FpsUtil::SetFPSLowerLimit(unsigned int fps )
{
	if (this->zMaxFps != 0 && fps < this->zMaxFps)
	{
		this->zMinFps = fps;
		this->zTargetLowerDelta = 1.0f / this->zMinFps;
	}
}

void FpsUtil::Update(float& deltaTime)
{
	if (this->zMinFps != 0)
	{
		this->Limit(deltaTime);
	}
	if (this->zMaxFps != 0)
	{
		this->SleepProgram(deltaTime);
	}
}

void FpsUtil::SleepProgram(float& deltaTime)
{
	int sleepTimeMil = 0;
	float intPart = 0.0f;
	float fractPart = 0.0f;
	float sleepTime = 0.0f;

	sleepTime = (this->zTargetUpperDelta - deltaTime) * 1000.0f;
	fractPart = modf(sleepTime, &intPart);

	if (sleepTime > 0.0f)
	{
		if(fractPart >= 0.5f)
			sleepTimeMil = ceilf(sleepTime);
		else
			sleepTimeMil = floorf(sleepTime);
	}

	//Sleep(sleepTimeMil);
}

void FpsUtil::Limit(float& deltaTime)
{
	//Check if Fps is Higher than minimum cap
	if (deltaTime > this->zTargetLowerDelta)
	{
		//Add difference to buffer.
		float currentDifference = deltaTime - this->zTargetLowerDelta;
		this->zLowerLimitAcc += currentDifference;
		//Decrease deltaTime so it doesn't go below limit.
		deltaTime -= currentDifference;
	}
	else
	{
		//Calculate difference Between current delta time and minimum limit.
		float difference = this->zTargetLowerDelta - deltaTime;

		//Check if Difference is higher than what is stored in Buffer.
		if (difference > this->zLowerLimitAcc)
		{
			//Add buffer to current deltaTime.
			deltaTime += this->zLowerLimitAcc;
			this->zLowerLimitAcc = 0.0f;
		}
		else
		{
			//Add difference to current deltaTime and decrease difference from buffer.
			deltaTime = difference;
			zLowerLimitAcc -= difference;
		}
	}
}