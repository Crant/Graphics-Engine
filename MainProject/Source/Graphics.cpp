#include "Graphics.h"

namespace 
{
	static GraphicsEngine* engine = NULL;
}

bool GraphicsInit( HINSTANCE hInstance )
{
	engine = CreateGraphicsEngine((unsigned int)hInstance, "Config.cfg");
	return (engine != 0);
}

GraphicsEngine* GetGraphics()
{
	if ( !engine )
		throw("Engine Not Initialized!");

	return engine;
}

bool FreeGraphics()
{
	if ( !engine )
		return false;

	delete engine;
	engine = 0;

	return true;
}