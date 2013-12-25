#pragma once

#include "DxManager.h"
#include "DirectX.h"
#include "KeyListener.h"
#include "GraphicsEngine.h"
#include "GraphicsEngineParameters.h"
#include "Camera.h"
#include "Process.h"
#include "GameTimer.h"
#include "FpsUtil.h"

class LoadMeshEvent : public CJen::ProcessEvent
{
private:
	std::string fileName;
	StaticMesh* mesh;
	bool selfdelete;

public:
	LoadMeshEvent(std::string fileName, StaticMesh* mesh) 
	{ 
		this->fileName = fileName; 
		this->mesh = mesh; 
		this->selfdelete = true;
	}
	virtual ~LoadMeshEvent() 
	{
		if(this->selfdelete) 
		{
			if(this->mesh) 
				delete this->mesh;
		}
	}
	std::string GetFileName() { return this->fileName; }
	StaticMesh* GetStaticMesh() { this->selfdelete = false; return this->mesh; }
};

class GraphicsEngineImpl : public CJen::Process, public GraphicsEngine
{
private:
	GraphicsEngineParams zParameters;
	DxManager* dx;
	Camera* zCamera;
	HINSTANCE hInstance;
	HWND hWnd;
	KeyListener* zKeyListener;
	GameTimer* zGameTimer;

	Image* zLoadingScreenBG;
	Image* zLoadingScreenPB;
	Image* zLoadingScreenFade;
	Text* zLoadingScreenPercentage;
	Text* zLoadingScreenText;
	
	int loadingScreenState;

	bool zLoadingScreen;
	bool zLoading;
	bool zRunning;
private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	void InitObjects();

public:
	GraphicsEngineImpl(const GraphicsEngineParams &params, HINSTANCE hInstance, int nCmdShow);
	virtual ~GraphicsEngineImpl();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//Get Functions
	GraphicsEngineParams& GetEngineParams() {return this->zParameters;}
	Camera* GetCamera() const {return this->zCamera;}
	HWND GetWindowHandle() const {return this->hWnd;}

	virtual void Life();

	// Skybox
	virtual void ChangeSkyBox(const char* texture);

	virtual iKeyListener* GetKeyListener() const {return this->zKeyListener;}
	virtual float Update();
	virtual bool IsRunning() {return this->zRunning;}
	virtual void StopRunning() {this->zRunning = false;}
	virtual void StartRendering() {this->dx->StartRendering();}
	virtual void SetFPSMax(float maxFPS) { this->dx->SetMaxFPS(maxFPS); }
	virtual void SetFPSMin(float minFPS) { this->dx->SetMinFPS(minFPS); }

	/*  Inherited from interface  */
	virtual iMesh* CreateStaticMesh(const char* filename, const Vector3& pos);
	virtual void DeleteMesh(iMesh* delMesh);

	// Create And Delete Terrain
	virtual iTerrain* CreateTerrain(const Vector3& pos, const Vector3& scale, const unsigned int& size, const char* heightMap);
	virtual void DeleteTerrain( iTerrain*& terrain );

	// Text
	virtual iText* CreateText(const char* text, Vector2 pos, float size, const char* fontTexturePath);
	virtual void DeleteText(iText* &deltxt);

	// Images
	virtual iImage* CreateImage(Vector2 pos, Vector2 dimensions, const char* texture);
	virtual void DeleteImage(iImage* &delImg);

	// Settings
	virtual iGraphicsEngineParams& GetEngineParameters();

	/*! 
	Takes control of the thread and renders a loading-screen with a progress bar. Returns once all objects that have been sent to load is loaded.
	To use it first make all CreateMesh()-calls that you need and then call LoadingScreen(.,.) directly after, and it will return once all the meshes are
	created and being rendered in the background. */
	virtual void LoadingScreen(const char* BackgroundTexture = "", const char* ProgressBarTexture = "", float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f, float FadeBlackOutInTime = 0.0f, float FadeBlackOutOutTime = 0.0f);
	virtual void ShowLoadingScreen(const char* BackgroundTexture = "", const char* ProgressBarTexture = "", float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f);
	virtual void HideLoadingScreen();

	/*! Create an Image and starts rendering it every frame. Return is a pointer to the image created. Coordinates are in screen-space. 
	To stop rendering it call DeleteImage on the engine with the image as parameter. */
	Image* CreateImage(D3DXVECTOR2 position, D3DXVECTOR2 dimensions, std::string texture);

	/*! Stops rendering the Image and internally deletes it and the pointer will become NULL. Return depends on if the Image was sucessfully removed. */
	bool DeleteImage(Image* delImage);

	Text* CreateText(std::string text, D3DXVECTOR2 position, float size, std::string fontTexturePath);
	bool DeleteText(Text* delText);

	virtual iPointLight* CreatePointLight(Vector3 pos, float radius, float intensity, bool enableShadows);
	virtual void DeletePointLight(iPointLight* &pLight);

	PointLight* CreatePointLight(D3DXVECTOR3 pos, float radius, float intensity, bool enableShadows);
	void DeletePointLight(PointLight* pLight);

	virtual void ResetPerfLogging();
	virtual void PrintPerfLogging();
};