#if defined(DEBUG) || defined(_DEBUG)
	#include <vld.h>
	#define INCLUDE_VLD
#endif

#include "Graphics.h"
#include "TestProgram.h"
#include "CjenFileDebug.h"

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

	TestProgram* program = new TestProgram(gEng);

	program->Run();

	delete program;

	gEng->PrintPerfLogging();
	// Free Graphics
	FreeGraphics();

	return 0;
}