#pragma once
#include "GraphicsEngine.h"
#include "GameTimer.h"

class TestProgram
{
private:
	GraphicsEngine* zGraphicsEngine;

	iMesh* zMesh;
	iTerrain* zTerrain;
	iWaterPlane* zWater;
	iPointLight* zPointLight;

	bool zFollowTerrain;

	float zDeltaTime;
private:
	void InitResources();
	void HandleMovement();
	void HandleInput();
public:
	TestProgram(GraphicsEngine* pGraphicsEngine);

	void Run();
};