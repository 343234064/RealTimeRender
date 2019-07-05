#pragma once
#include "Editor/Window.h"

struct WindowsDeferMessageArgs
{
	WindowsDeferMessageArgs(const SharedPTRWindow& Window, HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam, int32 X = 0, int32 Y = 0) :
		CurrentWindow(Window),
		hWnd(Hwnd),
		msg(Message),
		wParam(WParam),
		lParam(LParam),
		MouseX(X),
		MouseY(Y)
	{}

	SharedPTRWindow CurrentWindow;
	HWND hWnd;
	uint32 msg;
	WPARAM wParam;
	LPARAM lParam;
	int32 MouseX;
	int32 MouseY;
};
typedef WindowsDeferMessageArgs PlatformDeferMessageArgs;

class WinowsMessageHandler
{
public:
	virtual bool ProcessMessage(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& HandledResult) = 0;

};
typedef WinowsMessageHandler PlatformMessageHandler;