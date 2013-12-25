#include "GraphicsEngineParameters.h"
#include <fstream>
#include "CjenFileDebug.h"

void GraphicsEngineParams::LoadFromFile( const char* filename )
{
	std::fstream read;

	read.open(filename);

	if (!read.is_open())
	{
		CJen::Debug("Failed to load EngineParameters from " + std::string(filename) + ". Might be because it didn't exist, creating it now.");
		this->SaveToFile(filename);
		return;
	}
	std::string line;

	//Info
	std::getline(read, line);

	std::getline(read, line);
	this->WindowWidth = atoi(line.c_str());
	std::getline(read, line);
	std::getline(read, line);
	std::getline(read, line);
	this->WindowHeight = atoi(line.c_str());
	std::getline(read, line);
	std::getline(read, line);
	std::getline(read, line);
	this->FOV = atoi(line.c_str());
	std::getline(read, line);
	std::getline(read, line);
	std::getline(read, line);
	this->NearClip = (float)atof(line.c_str());
	std::getline(read, line);
	std::getline(read, line);
	std::getline(read, line);
	this->FarClip = atoi(line.c_str());
	std::getline(read, line);
	std::getline(read, line);
	std::getline(read, line);
	this->MaxFPS = (float)atof(line.c_str());
	std::getline(read, line);
	std::getline(read, line);
	std::getline(read, line);
	this->MinFps = (float)atof(line.c_str());
	std::getline(read, line);
	std::getline(read, line);
	std::getline(read, line);
	this->MouseSensativity = (float)atof(line.c_str());

	read.close();
}

void GraphicsEngineParams::SaveToFile( const char* filename )
{
	std::ofstream write;
	write.open(filename);
	if (!write.is_open())
	{
		CJen::Debug("Failed to save EngineParameters to " + std::string(filename));
		return;
	}

	write << "Window width:		Default: 1024" << endl;
	write << this->WindowWidth << std::endl;
	write << endl;
	write << "Window height:	Default: 768" << endl;
	write << this->WindowHeight << std::endl;
	write << endl;
	write << "Field of View:	Default: 75" << endl;
	write << this->FOV << std::endl;
	write << endl;
	write << "NearClip:		Default: 0.2" << endl;
	write << this->NearClip << std::endl;
	write << endl;
	write << "Far Clip:		Default: 200" << endl;
	write << this->FarClip << std::endl;
	write << endl;
	write << "Max FPS:		Default: 0 (unlimited)" << endl;
	write << this->MaxFPS << std::endl;
	write << endl;
	write << "Min FPS:		Default: 0 (unlimited)" << endl;
	write << this->MinFps << std::endl;
	write << endl;
	write << "Mouse Sensitivity:	Default: 1.0" << endl;
	write << this->MouseSensativity << std::endl;
	write << endl;
}