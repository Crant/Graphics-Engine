#include "GraphicsEngine.h"
#include "GraphicsEngineImpl.h"

DECLDIR GraphicsEngine* CreateGraphicsEngine( unsigned int hInstance, const char* configFile )
{
	GraphicsEngineParams GEP;
	GEP.LoadFromFile(configFile);

	GraphicsEngine* eng = new GraphicsEngineImpl(GEP, (HINSTANCE)hInstance, 1);
	return eng;
}