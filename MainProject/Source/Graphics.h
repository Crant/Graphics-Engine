#pragma once

#include <GraphicsEngine.h>
#include <windows.h>

#ifdef _DEBUG
#pragma comment(lib, "Graphics EngineD.lib")
#else
#pragma comment(lib, "Graphics Engine.lib")
#endif

bool GraphicsInit( HINSTANCE hInstance );
GraphicsEngine* GetGraphics();
bool FreeGraphics();