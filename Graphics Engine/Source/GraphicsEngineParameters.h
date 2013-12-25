#pragma once

#include <fstream>
#include <string>
#include "iGraphicsEngineParams.h"

class GraphicsEngineParams : public iGraphicsEngineParams
{
public:
	GraphicsEngineParams() {}
	virtual ~GraphicsEngineParams() {}

	void LoadFromFile(const char* filename);
	virtual void SaveToFile(const char* filename);
};