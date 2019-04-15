#pragma once

#include "Global/GlobalType.h"
#include "Global/Utilities/DynamicArray.h" 
#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include <memory>




//Main window instance handle
extern HINSTANCE gMainInstanceHandle;


#define PLATFORM_EDITOR_PUBLIC_FUNCTIONS \
   int32 MainMessageHandler(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam); 


#define PLATFORM_EDITOR_PROTECTED_FUNCTIONS \
   void  DeferMessage(SharedPTRWindow& CurrentWindow, HWND hWnd, uint32 msg,  WPARAM wParam, LPARAM lParam, int32 MouseX = 0, int32 MouseY = 0); \



#define PLATFORM_EDITOR_PROTECTED_VARIABLES \
   bool IsActivateByMouse; \
   bool IsEnterSizeMove; \


#define PLATFORM_EDITOR_MEMBER_INIT \
       IsActivateByMouse = false; \
       IsEnterSizeMove = false;

#define PLATFORM_EDITOR_MEMBER_CLEANUP





struct WindowsDeferMessageArgs
{
	WindowsDeferMessageArgs(const SharedPTRWindow& Window, HWND Hwnd, uint32 Message, WPARAM WParam, LPARAM LParam, int32 X = 0, int32 Y = 0):
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
typedef std::shared_ptr<WinowsMessageHandler> SharedPTRMessageHandler;







