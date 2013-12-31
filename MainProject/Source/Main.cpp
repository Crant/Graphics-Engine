#if defined(DEBUG) || defined(_DEBUG)
	#include <vld.h>
	#define INCLUDE_VLD
#endif

#include "Graphics.h"
#include "CjenFileDebug.h"
#include "GameTimer.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	CJen::ClearDebug();

 #if defined(DEBUG) || defined(_DEBUG)
 	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 	CJen::Debug("(DEBUG): MainProject: Debug flag set to: _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF). ");
 #ifdef INCLUDE_VLD
 	CJen::Debug("(DEBUG): MainProject: vld.h included.");
 #endif
 #endif
	
	std::string errorMsg;

	if(!GraphicsInit(hInstance))
	{
		CJen::Debug(__FILE__);
		
		errorMsg = "Failed Initializing Graphics! Line: ";
		CJen::addNrToString(__LINE__, errorMsg);
		CJen::Debug(errorMsg + "\n");
		return 1;
	}

	GraphicsEngine* gEng = GetGraphics();
	gEng->ResetPerfLogging();

	gEng->ShowLoadingScreen("media/loadingScreen/loadingscreenbg.png", "media/loadingscreen/loadingscreenpb.png", 1.0f, 0.2f);

	iMesh* mesh = gEng->CreateStaticMesh("media/tree_01.obj", Vector3(3, 0, 0));
	mesh->SetScale(Vector3(0.01f, 0.01f, 0.01f));
	
	gEng->CreateTerrain(Vector3(0, 0, 0), Vector3(15.0f, 1.0f, 15.0f), 256, "media/Terrain/TerrainOne.raw");
	//gEng->CreateTerrain(Vector3(0.0f, 0.0f, 10.0f), Vector3(10.0f, 1.0f, 10.0f), 256, "");

	//iPointLight* pLight1 = gEng->CreatePointLight(Vector3(-3.0f, 1.0f, 0.0f), 2.0f, 1.0f, true);
	//pLight1->SetColor(Vector3(255.0f, 255.0f, 0.0f));

	iPointLight* pLight2 = gEng->CreatePointLight(Vector3(1.4f, 1.0f, 0.0f), 2.0f, 1.0f, false);
	pLight2->SetColor(Vector3(0.0f, 255.0f, 255.0f));
	
	iSpotLight* sLight = gEng->CreateSpotLight(gEng->GetCamera()->GetPosition(), gEng->GetCamera()->GetForward(), 1.0f, true);
	sLight->SetColor(Vector3(255.0f, 0.0f, 255.0f));

	gEng->GetCamera()->SetPosition(Vector3(0, 5, 0));
	gEng->GetCamera()->LookAt(Vector3(10, 0, 0));
	bool walk = false;
	gEng->GetCamera()->SetFollowTerrain(walk);
	gEng->ChangeSkyBox("Media/SkyMap.dds");

	gEng->LoadingScreen("media/loadingScreen/loadingscreenbg.png", "media/loadingscreen/loadingscreenpb.png", 0.0f, 0.2f, 0.2f, 0.2f);

	float deltaTime = 0.0f;
	bool increase = true;

	while (gEng->IsRunning())
	{
		deltaTime = gEng->Update();
		
		if (gEng->GetKeyListener()->IsPressed('W'))
		{
			gEng->GetCamera()->MoveForward(deltaTime);
		}
		if (gEng->GetKeyListener()->IsPressed('S'))
		{
			gEng->GetCamera()->MoveBackward(deltaTime);
		}
		if (gEng->GetKeyListener()->IsPressed('A'))
		{
			gEng->GetCamera()->MoveLeft(deltaTime);
		}
		if (gEng->GetKeyListener()->IsPressed('D'))
		{
			gEng->GetCamera()->MoveRight(deltaTime);
		}
		if (gEng->GetKeyListener()->IsPressed('D'))
		{
			gEng->GetCamera()->MoveRight(deltaTime);
		}
		if (gEng->GetKeyListener()->IsPressed('Q'))
		{
			walk = !walk;
			gEng->GetCamera()->SetFollowTerrain(walk);
		}
		if (gEng->GetKeyListener()->IsPressed(VK_SPACE))
		{
			gEng->GetCamera()->MoveUp(deltaTime);
		}
		if (gEng->GetKeyListener()->IsPressed(VK_ESCAPE))
		{
			gEng->StopRunning();
		}
		if(gEng->GetKeyListener()->IsClicked(2))
		{
			gEng->GetCamera()->SetUpdate(true);
		}
		else
		{
			gEng->GetCamera()->SetUpdate(false);
		}
		if(gEng->GetKeyListener()->IsPressed('I'))
		{
			//pLight1->SetRadius(pLight1->GetRadius()+0.3f*deltaTime);
			pLight2->SetRadius(pLight2->GetRadius()+0.3f*deltaTime);
		}
		if(gEng->GetKeyListener()->IsPressed('O'))
		{
			//pLight1->SetRadius(pLight1->GetRadius()-0.3f*deltaTime);
			pLight2->SetRadius(pLight2->GetRadius()-0.3f*deltaTime);
		}
		
	}

	gEng->DeleteMesh(mesh);

	gEng->PrintPerfLogging();
	// Free Graphics
	FreeGraphics();

	return 0;
}

void Init()
{


}