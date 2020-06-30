#pragma once

#include "Global/GlobalType.h"
#include "Global/Utilities/DynamicArray.h" 
#include "HAL/Platform.h"
#include "Editor/Platforms/PlatformEditor.h"
#include <memory>



class WindowsEditor: public PlatformEditorInterface, public ActionCallback
{
public:
	WindowsEditor():
		IsActivateByMouse(false),
		IsEnterSizeMove(false)
	{}
	virtual ~WindowsEditor()
	{
		if (IsInitialized)
		{
			Exit();
		}
	}

	bool Init() override;
	void Tick(const float DeltaTime) override;
	void Exit() override;

	int32 MainMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool  AddNewWindow(const SharedPTRWindow& ParentWindow, const SharedPTRWinDescription& Description, bool ShowImmediately, bool FocusImmediately) override;
	int32 ProcessDeferredMessage(const PlatformDeferMessageArgs& Message) override;
	void  PumpMessages(const float TimeDelta) override;


	


protected:
	void  DeferMessage(SharedPTRWindow& CurrentWindow, HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32 MouseX = 0, int32 MouseY = 0);


protected:
	bool IsActivateByMouse;
	bool IsEnterSizeMove;

};


typedef WindowsEditor PlatformEditor;
typedef std::unique_ptr<PlatformEditor> PTRPlatformEditor;




