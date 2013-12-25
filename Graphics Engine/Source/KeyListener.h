#pragma once

#include "DirectX.h"
#include "Process.h"
#include "iKeyListener.h"

class KeyPressedEvent : public CJen::ProcessEvent
{
private:
	WPARAM key;
	bool zPressed;

public:
	KeyPressedEvent(WPARAM key, bool pressed)
	{
		this->key = key;
		this->zPressed = pressed;
	}
	virtual ~KeyPressedEvent() {}
	WPARAM GetKey() const {return this->key;}
	bool GetPressed() const {return this->zPressed;}
};

class MouseClickEvent : public CJen::ProcessEvent
{
private:
	int button;
	bool pressed;

public:
	MouseClickEvent(int button, bool pressed) { this->button = button; this->pressed = pressed; }
	virtual ~MouseClickEvent() {}
	int GetButton() const { return this->button; }
	bool GetPressed() const { return this->pressed; }
};

class KeyListener : public iKeyListener
{
private:
	bool keys[256];
	bool mouse[50]; // Support 50 buttons
	CJen::Process* notifier;
	HWND hwnd;

public:
	KeyListener(HWND handle, CJen::Process* notifier = NULL);
	virtual ~KeyListener() { }

	virtual void KeyDown(WPARAM param);
	virtual void KeyUp(WPARAM param);

	virtual void MouseDown(int button);
	virtual void MouseUp(int button);

	virtual bool IsPressed(char key);
	virtual bool IsPressed(int key);
	virtual bool IsClicked(int button);
	//bool HasBeenPressedSinceLast(char NotYetImplemented) { }

	/*! Returns the mouse-position relative to the window (excluding borders) */
	virtual Vector2 GetMousePosition() const;

	/*! Sets the mouse-position relative to the window (excluding borders) */
	virtual void SetMousePosition(const Vector2& mousePos);

	virtual void SetCursorVisibility(bool visi);

	void SetHWND(HWND hWnd) { this->hwnd = hWnd; }
};