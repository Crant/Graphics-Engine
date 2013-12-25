#include "GameTimer.h"

GameTimer::GameTimer()
{
	this->zRunTime = 0.0f;
	this->zDeltaTime = 0.0f;
	this->zSecsPerCnt = 0.0f;

	this->zFramesPerSec = 0;
	this->zFrameCounter = 0;

	this->zFpsUtility = NULL;
}

GameTimer::~GameTimer()
{
	if (this->zFpsUtility)
		delete this->zFpsUtility;

	this->zFpsUtility = NULL;
}

void GameTimer::Init()
{
	INT64 frequency;

	QueryPerformanceFrequency( (LARGE_INTEGER*) &frequency);

	this->zSecsPerCnt = 1.0f / (float)(frequency);
	
	QueryPerformanceCounter( (LARGE_INTEGER*) &this->zStartTime);

	//this->zFpsUtility = new FpsUtil();
}

float GameTimer::Frame()
{
	//Calculate DeltaTime
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter( (LARGE_INTEGER*) &currentTime);

	timeDifference = (float) (currentTime - this->zStartTime);

	this->zDeltaTime = timeDifference * this->zSecsPerCnt;

	this->zRunTime += this->zDeltaTime;

	this->zStartTime = currentTime;

	//this->zFpsUtility->Update(this->zDeltaTime);

	this->CalculateFps();

	return this->zDeltaTime;
}

void GameTimer::CalculateFps()
{
	static const unsigned int MAX_SAMPLES = 50;

	if (this->zDt.size() >= MAX_SAMPLES)
		this->zDt.erase(this->zDt.begin());

	this->zDt.push_back(this->zDeltaTime);

	float totalDeltaTime = 0.0f;

	auto it_end = this->zDt.end();
	for (auto it = this->zDt.begin(); it != it_end; it++)
		totalDeltaTime += (*it);

	float averageDelta = totalDeltaTime / this->zDt.size();

	this->zFramesPerSec = (int)(1.0f / averageDelta);
}