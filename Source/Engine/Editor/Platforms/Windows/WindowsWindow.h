#pragma once

#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "Editor/Window.h"





class WindowsWindow :public WindowInterface
{
public:
	WindowsWindow() :
		Hwnd(NULL),
		//RegionWidth(0),
		//RegionHeight(0),
		ActualWidth(0),
	    ActualHeight(0),
		AspectRatio(1.0f),
		IsVisibled(false),
		IsInitialized(false),
		HasNeverShow(false)
	{
		Memory::Zero(&PreParentMinimizedWindowPlacement, sizeof(PreParentMinimizedWindowPlacement));
	}

	virtual ~WindowsWindow() 
	{
		if (IsInitialized)
			Destroy();
	}

	bool Create(const SharedPTRWinDescription& Description, const std::shared_ptr<WindowInterface> Parent) override;
	void Destroy() override;
	void Show() override;
	void Hide() override;
	void Restore() override;
	void Focus() override;
	//void Tick(const float TimeDelta = 0) override;
	void Minimize() override;
	void Maximize() override;
	void Reshape(int32 ClientX, int32 ClientY, int32 ClientWidth, int32 ClientHeight) override;
	void MoveWindowTo(int32 ClientX, int32 ClientY) override;

	bool IsMinimized() override { return !!::IsZoomed(Hwnd); }
	bool IsMaximized() override { return !!::IsIconic(Hwnd); }
	bool IsVisible() override { return IsVisibled; }
	bool IsInit() override { return IsInitialized; }

	void SetTitle(const TChar* Text) override { ::SetWindowText(Hwnd, Text); }
	void SetWindowMode(WindowMode NewMode) override;
	void SetOpacity(const float Opacity) override { ::SetLayeredWindowAttributes(Hwnd, 0, (BYTE)Platform::TruncToInt(Opacity * 255.0f), LWA_ALPHA); }

	void  GetClientSize(int32& OutWidth, int32& OutHeight) override;
	int32 GetBorderSize() override;
	int32 GetTitleBarSize() override { return GetSystemMetrics(SM_CYCAPTION); }
	void* GetWindowHandle() override { return (void*)Hwnd; }
	float GetAspectRatio() override { return AspectRatio; }
	WindowMode GetWindowMode() override { return WindowDesc->WinMode; }
	
	void OnParentWindowMinimized() override;
	void OnParentWindowRestored() override;
	
	//Window Region is the window size we see in monitor
	void AdjustWindowRegion(int32 Width, int32 Height) override;

//Platform members
public:


protected:
	HWND Hwnd;

	//Create a larger window and cut the Windows size so that it appears smaller, 
	//to advoid actually resizing it and incurring a GPU buffer resize performance hit
	//The window client size we can see
	//int32 RegionWidth;
	//int32 RegionHeight;
	//The actual window size
	int32 ActualWidth;
	int32 ActualHeight;

	float AspectRatio;

	bool IsVisibled;
	bool IsInitialized;
	//The window hasn't been showed yet
	bool HasNeverShow;

	//The placement of the window, used to store minimized state when its parent window being minimized
	WINDOWPLACEMENT PreParentMinimizedWindowPlacement;
};