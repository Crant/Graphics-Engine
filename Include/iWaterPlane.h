#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

extern "C"
{
	class DECLDIR iWaterPlane
	{	
	public:
		iWaterPlane() {};
		virtual ~iWaterPlane() {};

	};
}