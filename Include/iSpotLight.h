#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

extern "C"
{
	class DECLDIR iSpotLight
	{	
	public:
		iSpotLight() {};
		virtual ~iSpotLight() {};

		virtual float GetIntensity() = 0;

		virtual Vector3 GetColor() = 0;
		virtual Vector3 GetPosition() = 0;
		virtual Vector3 GetDirection() = 0;

		virtual void SetIntensity(float intensity) = 0;

		/*! Color is set between 0 - 255 and normalized internally
		Default Color is (255, 255, 255).*/
		virtual void SetColor(const Vector3& color) = 0;
		/*! Set Center position of Light.*/
		virtual void SetPosition(const Vector3& position) = 0;
		virtual void SetDirection(const Vector3& direction) = 0;

		virtual bool IsShadowsEnabled() = 0;
		/*! Enable or Disable shadows.*/
		virtual void SetShadows(const bool& value) = 0;
	};
}