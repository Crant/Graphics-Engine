#pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include <iCamera.h>
#include <iMesh.h>
#include <iKeyListener.h>
#include <iTerrain.h>
#include <iText.h>
#include <iImage.h>
#include <iPointLight.h>

extern "C"
{
	class DECLDIR GraphicsEngine
	{
	public:
		GraphicsEngine() {}
		virtual ~GraphicsEngine() {}

		// Skybox
		virtual void ChangeSkyBox(const char* texture) = 0;

		virtual iCamera* GetCamera() const = 0;
		virtual iKeyListener* GetKeyListener() const = 0;

		/*! Updates the Camera and takes care of all key-inputs and returns diff in seconds (ie. 0.000065f seconds) */
		virtual float Update() = 0;

		virtual void SetFPSMax(float maxFPS) = 0;
		virtual void SetFPSMin(float minFPS) = 0;

		virtual void StartRendering() = 0;
		virtual void ResetPerfLogging() = 0;
		virtual void PrintPerfLogging() = 0;

		virtual bool IsRunning() = 0;
		virtual void StopRunning() = 0;

		virtual void LoadingScreen(const char* BackgroundTexture = "", const char* ProgressBarTexture = "", 
			float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f, float FadeBlackOutInTime = 0.0f, 
			float FadeBlackOutOutTime = 0.0f) = 0;

		virtual void ShowLoadingScreen(const char* BackgroundTexture = "", const char* ProgressBarTexture = "", 
			float FadeBlackInInTime = 0.0f, float FadeBlackInOutTime = 0.0f) = 0;

		virtual void HideLoadingScreen() = 0;

		virtual iGraphicsEngineParams& GetEngineParameters() = 0;

		virtual iMesh* CreateStaticMesh(const char* filename, const Vector3& pos) = 0;
		virtual void DeleteMesh(iMesh* delMesh) = 0;

		// Create And Delete Terrain
		virtual iTerrain* CreateTerrain(const Vector3& pos, const Vector3& scale, const unsigned int& size, const char* heightMap) = 0;
		virtual void DeleteTerrain( iTerrain*& terrain ) = 0;

		virtual iImage* CreateImage(Vector2 pos, Vector2 dimensions, const char* texture) = 0;
		virtual void DeleteImage(iImage* &delImg) = 0;

		virtual iText* CreateText(const char* text, Vector2 pos, float size, const char* fontTexturePath) = 0;
		virtual void DeleteText(iText* &deltxt) = 0;

		virtual iPointLight* CreatePointLight(Vector3 pos, float radius, float intensity, bool enableShadows = false) = 0;
		virtual void DeletePointLight(iPointLight* &pLight) = 0;
	};

	DECLDIR GraphicsEngine* CreateGraphicsEngine(unsigned int hInstance, const char* configFile);
};