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

extern "C"
{
	class DECLDIR GraphicsEngine
	{
	public:
		GraphicsEngine() {}
		virtual ~GraphicsEngine() {}

		virtual iCamera* GetCamera() const = 0;
		virtual iKeyListener* GetKeyListener() const = 0;

		/*! Updates the Camera and takes care of all key-inputs and returns diff in seconds (ie. 0.000065f seconds) */
		virtual float Update() = 0;

		virtual void SetFPSMax(float maxFPS) = 0;

		virtual void StartRendering() = 0;

		virtual bool IsRunning() = 0;

		virtual iMesh* CreateStaticMesh(const char* filename, const Vector3& pos) = 0;
		virtual void DeleteMesh(iMesh* delMesh) = 0;
	};

	DECLDIR GraphicsEngine* CreateGraphicsEngine(unsigned int hInstance, const char* configFile);
};