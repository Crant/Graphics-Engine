#include "TestProgram.h"

TestProgram::TestProgram( GraphicsEngine* pGraphicsEngine )
{
	this->zGraphicsEngine = pGraphicsEngine;
}

void TestProgram::Run()
{
	this->zGraphicsEngine->ShowLoadingScreen("media/loadingScreen/loadingscreenbg.png", "media/loadingscreen/loadingscreenpb.png", 1.0f, 0.2f);

	this->InitResources();

	this->zGraphicsEngine->LoadingScreen("media/loadingScreen/loadingscreenbg.png", "media/loadingscreen/loadingscreenpb.png", 0.0f, 0.2f, 0.2f, 0.2f);

	while (this->zGraphicsEngine->IsRunning())
	{
		this->zDeltaTime = this->zGraphicsEngine->Update();
		
		this->HandleInput();

	}

	this->zGraphicsEngine->DeleteMesh(this->zMesh);
}

void TestProgram::InitResources()
{
	//iMesh* LightPos = this->zEngine->CreateStaticMesh("media/Stone_01.obj", Vector3(3, 0, 0));
	//LightPos->SetScale(Vector3(0.01f, 0.01f, 0.01f));

	this->zMesh = this->zGraphicsEngine->CreateStaticMesh("media/Stone_01.obj", Vector3(3, 0, 0));
	this->zMesh->SetScale(Vector3(0.1f, 0.1f, 0.1f));

	this->zTerrain = this->zGraphicsEngine->CreateTerrain(Vector3(0, 0, 0), Vector3(25.0f, 1.0f, 25.0f), 256, "media/Terrain/TerrainOne.raw");
	//this->zEngine->CreateTerrain(Vector3(0.0f, 0.0f, 10.0f), Vector3(10.0f, 1.0f, 10.0f), 256, "");

	//iPointLight* pLight1 = this->zEngine->CreatePointLight(Vector3(-3.0f, 1.0f, 0.0f), 2.0f, 1.0f, true);
	//pLight1->SetColor(Vector3(255.0f, 255.0f, 0.0f));

	this->zPointLight = this->zGraphicsEngine->CreatePointLight(Vector3(1.4f, 2.0f, 0.0f), 10.0f, 0.2f, false);
	this->zPointLight->SetColor(Vector3(0.0f, 255.0f, 255.0f));

	//iSpotLight* sLight = this->zEngine->CreateSpotLight(this->zEngine->GetCamera()->GetPosition(), this->zEngine->GetCamera()->GetForward(), 1.0f, true);
	//sLight->SetColor(Vector3(255.0f, 0.0f, 255.0f));

	Vector3 points[4] = {Vector3(0, 0, 0), Vector3(25.0f, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 25.0f)};

	iWaterPlane* wPlane = this->zGraphicsEngine->CreateWaterPlane(points);

	this->zGraphicsEngine->GetCamera()->SetPosition(Vector3(0, 5, 0));
	this->zGraphicsEngine->GetCamera()->LookAt(Vector3(10, 0, 0));
	this->zGraphicsEngine->GetCamera()->SetFollowTerrain(false);
	this->zGraphicsEngine->ChangeSkyBox("Media/SkyMap.dds");
}

void TestProgram::HandleMovement()
{
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed(VK_UP))
	{
		this->zPointLight->SetPosition(this->zPointLight->GetPosition() + Vector3(1, 0, 0) * this->zDeltaTime);
		this->zGraphicsEngine->GetCamera()->SetPosition(this->zPointLight->GetPosition());
	}
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed(VK_DOWN))
	{
		this->zPointLight->SetPosition(this->zPointLight->GetPosition() + Vector3(-1, 0, 0) * this->zDeltaTime);
		this->zGraphicsEngine->GetCamera()->SetPosition(this->zPointLight->GetPosition());
	}
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed(VK_LEFT))
	{
		this->zPointLight->SetPosition(this->zPointLight->GetPosition() + Vector3(0, 0, 1) * this->zDeltaTime);
		this->zGraphicsEngine->GetCamera()->SetPosition(this->zPointLight->GetPosition());
	}
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed(VK_RIGHT))
	{
		this->zPointLight->SetPosition(this->zPointLight->GetPosition() + Vector3(0, 0, -1) * this->zDeltaTime);
		this->zGraphicsEngine->GetCamera()->SetPosition(this->zPointLight->GetPosition());
	}
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed('W'))
	{
		this->zGraphicsEngine->GetCamera()->MoveForward(this->zDeltaTime);
	}
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed('S'))
	{
		this->zGraphicsEngine->GetCamera()->MoveBackward(this->zDeltaTime);
	}
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed('A'))
	{
		this->zGraphicsEngine->GetCamera()->MoveLeft(this->zDeltaTime);
	}
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed('D'))
	{
		this->zGraphicsEngine->GetCamera()->MoveRight(this->zDeltaTime);
	}

	if (this->zGraphicsEngine->GetKeyListener()->IsPressed(VK_SPACE))
	{
		//this->zEngine->GetCamera()->MoveUp(deltaTime);
		this->zPointLight->SetPosition(this->zPointLight->GetPosition() + Vector3(0, 1, 0) * this->zDeltaTime);
		//LightPos->SetPosition(pLight2->GetPosition());
	}
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed(VK_DELETE))
	{
		//this->zEngine->GetCamera()->MoveUp(deltaTime);
		this->zPointLight->SetPosition(this->zPointLight->GetPosition() + Vector3(0, -1, 0) * this->zDeltaTime);
		//LightPos->SetPosition(pLight2->GetPosition());
	}
}

void TestProgram::HandleInput()
{
	this->HandleMovement();

	if (this->zGraphicsEngine->GetKeyListener()->IsPressed('Q'))
	{
		this->zFollowTerrain = !this->zFollowTerrain;
		this->zGraphicsEngine->GetCamera()->SetFollowTerrain(this->zFollowTerrain);
	}
	
	if (this->zGraphicsEngine->GetKeyListener()->IsPressed(VK_ESCAPE))
	{
		this->zGraphicsEngine->StopRunning();
	}
	if(this->zGraphicsEngine->GetKeyListener()->IsClicked(2))
	{
		this->zGraphicsEngine->GetCamera()->SetUpdate(true);
	}
	else
	{
		this->zGraphicsEngine->GetCamera()->SetUpdate(false);
	}
	if(this->zGraphicsEngine->GetKeyListener()->IsPressed('I'))
	{
		//pLight1->SetRadius(pLight1->GetRadius()+0.3f*deltaTime);
		this->zPointLight->SetRadius(this->zPointLight->GetRadius() + 0.3f * this->zDeltaTime);
	}
	if(this->zGraphicsEngine->GetKeyListener()->IsPressed('O'))
	{
		//pLight1->SetRadius(pLight1->GetRadius()-0.3f*deltaTime);
		this->zPointLight->SetRadius(this->zPointLight->GetRadius() - 0.3f * this->zDeltaTime);
	}
}

