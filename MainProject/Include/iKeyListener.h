#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include <Vector.h>

extern "C"
{
	class DECLDIR iKeyListener
	{	
	public:
		iKeyListener() {};
		virtual ~iKeyListener() {};

		virtual void KeyDown(unsigned int param) = 0;
		virtual void KeyUp(unsigned int param) = 0;

		virtual void MouseDown(int button) = 0;
		virtual void MouseUp(int button) = 0;

		virtual bool IsPressed(char key) = 0;
		virtual bool IsPressed(int key) = 0;
		virtual bool IsClicked(int button) = 0;

		/*! Returns the mouse-position relative to the window (excluding borders) */
		virtual Vector2 GetMousePosition() const = 0;

		/*! Sets the mouse-position relative to the window (excluding borders) */
		virtual void SetMousePosition(const Vector2& mousePos) = 0;

		virtual void SetCursorVisibility(bool visi) = 0;
	};
}