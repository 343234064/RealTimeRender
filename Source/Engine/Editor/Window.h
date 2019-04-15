/************************************
Window 


*************************************/
#pragma once

#include "Global/GlobalType.h"
#include "Global/Utilities/String.h"

#include <memory>


enum class TitleAlignment
{
	//Aligned to the left
	Left,
	//Centered
	Center,
	//Aligned to the right
	Right

};

enum class WindowMode
{
	//FullScreen,

	//Standard Windowed
	Windowed,

	//Windowed but no border
	Borderless
};


enum class WindowActivation
{
	//Window never activates when it is shown
	Never,
	//Window always activates when it is shown
	Always,
	//Window activates when it is first shown
	First
};


struct WindowDescription
{
	//Desired client pos on screen space
	int32 DesiredPosXOnScreen;
	int32 DesiredPosYOnScreen;

	//Desired client size on screen space
	int32 DesiredWidthOnScreen;
	int32 DesiredHeightOnScreen;

	//Desired Client size limit 
	int32 DesiredMinWidth;
	int32 DesiredMaxWidth;

	int32 DesiredMinHeight;
	int32 DesiredMaxHeight;

	
	//Window transparency
	//This value will make the entire window get transparent when HasBorder = true
	float Opacity;

	//If the window accept input
	bool AcceptsInput;

	//If the window show in taskbar
	bool AppearInTaskBar;

	//If the window is on top of all window
	bool TopMostWindow;

	bool HasCloseButton;
	bool HasSizingBorder;

	bool SupportMinimize;
	bool SupportMaximize;
	
	//Maximize when first show
	bool InitiallyMaximized;

	//If the window keeps its AspectRatio when rezied by user
	bool KeepAspectRatio;

	WindowActivation ActivationPolicy;

	WindowMode WinMode;

	String Title;


	WindowDescription():
		DesiredPosXOnScreen(0),
		DesiredPosYOnScreen(0),
		DesiredWidthOnScreen(0),
		DesiredHeightOnScreen(0),
		DesiredMinWidth(0),
		DesiredMaxWidth(0),
		DesiredMinHeight(0),
		DesiredMaxHeight(0),
		Opacity(1.0f),
		AcceptsInput(false),
		AppearInTaskBar(false),
		TopMostWindow(false),
		HasCloseButton(false),
		HasSizingBorder(false),
		SupportMinimize(false),
		SupportMaximize(false),
		InitiallyMaximized(false),
		KeepAspectRatio(false),
		ActivationPolicy(WindowActivation::Always),
		WinMode(WindowMode::Windowed),
		Title()
	{}
};
typedef std::shared_ptr<WindowDescription> SharedPTRWinDescription;



class WindowInterface
{
public:
	WindowInterface():
		WindowDesc(nullptr)
	{}

	virtual ~WindowInterface()
	{
		WindowDesc = nullptr;
	}

	virtual bool Create(const SharedPTRWinDescription& Description, const std::shared_ptr<WindowInterface> Parent) = 0;
	virtual void Destroy() = 0;
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void Restore() = 0;
	virtual void Focus() = 0;
	virtual void Tick(const float TimeDelta = 0) {}
	virtual void Minimize() = 0;
	virtual void Maximize() = 0;
	virtual void Reshape(int32 ClientX, int32 ClientY, int32 ClientWidth, int32 ClientHeight) = 0;
	virtual void MoveWindowTo(int32 ClientX, int32 ClientY) = 0;

	virtual bool IsMinimized() = 0;
	virtual bool IsMaximized() = 0;
	virtual bool IsVisible() = 0;
	virtual bool IsInit() = 0;

	virtual void SetWindowMode(WindowMode NewMode) = 0;
	virtual void SetTitle(const TChar* Text) = 0;
	virtual void SetOpacity(const float Opacity) {}

	virtual void GetClientSize(int32& OutWidth, int32& OutHeight) = 0;
	virtual int32 GetBorderSize() = 0;
	virtual int32 GetTitleBarSize() = 0;
	virtual void* GetWindowHandle() = 0;
	virtual float GetAspectRatio() = 0;
	virtual SharedPTRWinDescription GetDescription() { return WindowDesc; }
	virtual WindowMode GetWindowMode() = 0;

	virtual void OnParentWindowMinimized() {}
	virtual void OnParentWindowRestored() {}

	virtual void AdjustWindowRegion(int32 Width, int32 Height) {}

protected:
	SharedPTRWinDescription WindowDesc;
};


typedef std::shared_ptr<WindowInterface> SharedPTRWindow;

