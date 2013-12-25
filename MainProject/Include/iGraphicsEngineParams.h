#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

extern "C"
{
	class DECLDIR iGraphicsEngineParams
	{
	public:
		iGraphicsEngineParams() {}
		virtual ~iGraphicsEngineParams() {}

		static int WindowWidth;
		static int WindowHeight;
		static bool Maximized;
		static int RefreshRate;
		static float MaxFPS;
		static float FOV;
		static float NearClip;
		static float FarClip;
		static float MouseSensativity;

		virtual void SaveToFile(const char* file) = 0;
	};
};