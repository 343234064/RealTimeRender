#include "Editor/Platforms/Windows/WindowsWindow.h"
#include "Editor/Platforms/Windows/WindowsEditorMisc.h"
#include "Global/EngineVariables.h"

#include <dwmapi.h>
#pragma comment(lib,"dwmapi")



bool WindowsWindow::Create(const SharedPTRWinDescription& Description, const std::shared_ptr<WindowInterface> Parent)
{
	WindowDesc = Description;

	const float InitPosX = (float)WindowDesc->DesiredPosXOnScreen;
	const float InitPosY = (float)WindowDesc->DesiredPosYOnScreen;
	const float InitWidth = (float)Clamp(WindowDesc->DesiredWidthOnScreen, WindowDesc->DesiredMinWidth, WindowDesc->DesiredMaxWidth);
	const float InitHeight = (float)Clamp(WindowDesc->DesiredHeightOnScreen, WindowDesc->DesiredMinHeight, WindowDesc->DesiredMaxHeight);

	int32 ClientX = Platform::TruncToInt(InitPosX);
	int32 ClientY = Platform::TruncToInt(InitPosY);
	int32 ClientWidth = Platform::TruncToInt(InitWidth);
	int32 ClientHeight = Platform::TruncToInt(InitHeight);

	int32 WindowX = ClientX;
	int32 WindowY = ClientY;
	int32 WindowWidth = ClientWidth;
	int32 WindowHeight = ClientHeight;

	uint32 WinExStyle = 0;
	uint32 WinStyle = 0;


	if (WindowDesc->WinMode == WindowMode::Borderless)
	{
		//The window has a border with a raised edge.
		WinExStyle = WS_EX_WINDOWEDGE;

		//Paints all descendants of a window in bottom-to-top painting order using double-buffering
		WinExStyle |= WS_EX_COMPOSITED;

		WinStyle = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

		if(WindowDesc->AppearInTaskBar)
		    //Forces a top-level window onto the taskbar when the window is visible
		    WinExStyle |= WS_EX_APPWINDOW;
		else
			//The window is palette window, which is a modeless dialog box that presents an array of commands. 
			WinExStyle |= WS_EX_TOOLWINDOW;

		if(!WindowDesc->AcceptsInput)
			//The window should not be painted until siblings beneath the window have been painted
			WinExStyle |= WS_EX_TRANSPARENT;

		if (WindowDesc->TopMostWindow)
		    //The window should be placed above all non-topmost windows and should stay above them
			WinExStyle |= WS_EX_TOPMOST;

	}
	else
	{
		//Standard window
		WinExStyle = WS_EX_APPWINDOW;
		WinStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;//Overlap and title bar

		WinExStyle |= WS_EX_LAYERED;

		if(WindowDesc->SupportMinimize)
			WinStyle |= WS_MINIMIZEBOX;

		if (WindowDesc->SupportMaximize)
			WinStyle |= WS_MAXIMIZEBOX;
	
		if (WindowDesc->HasSizingBorder)
			WinStyle |= WS_THICKFRAME;
		else
			WinStyle |= WS_BORDER;

		//Get border size
		RECT Rect = { 0,0,0,0 };
		::AdjustWindowRectEx(&Rect, WinStyle, false, WinExStyle);

		//Rect size is negative
		WindowX += Rect.left;
		WindowY += Rect.right;

		WindowWidth += Rect.right - Rect.left;
		WindowHeight += Rect.bottom - Rect.top;

	}

	
	Hwnd = CreateWindowEx(
		WinExStyle,
		APPLICATION_IDENTITY,
		*WindowDesc->Title,
		WinStyle,
		WindowX, WindowY,
		WindowWidth, WindowHeight,
		(Parent != nullptr) ? (HWND)Parent->GetWindowHandle() : NULL,
		NULL,
		gMainInstanceHandle,
		NULL
	);

	if (Hwnd == NULL)
	{
		MessageBox(NULL, TEXT("Window Creation Failed"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
		//log failed
		return false;
	}

	//ActualWidth = ClientWidth;
	//ActualHeight = ClientHeight;

	Reshape(ClientX, ClientY, ClientWidth, ClientHeight);

	SetOpacity(WindowDesc->Opacity);
	
	if (WindowDesc->WinMode == WindowMode::Borderless)
	{
		//Disable window border effect rendering  
		const DWMNCRENDERINGPOLICY Policy = DWMNCRP_DISABLED;
		if (FAILED(DwmSetWindowAttribute(Hwnd, DWMWA_NCRENDERING_POLICY, &Policy, sizeof(Policy))))
		{
			//log
		}

		//Disable content rendered in the non-client area to be visible on the frame drawn by DWM
		const BOOL NCPaint = false;
		if (FAILED(DwmSetWindowAttribute(Hwnd, DWMWA_ALLOW_NCPAINT, &NCPaint, sizeof(NCPaint))))
		{
			//log
		}

		//Extend the client area to border
		const MARGINS Margins = { -1 };
		if (FAILED(DwmExtendFrameIntoClientArea(Hwnd, &Margins)))
		{
			//log
		}
		

		WinStyle |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
		if (WindowDesc->SupportMaximize)
		{
			WinStyle |= WS_MAXIMIZEBOX;
		}
		if (WindowDesc->SupportMinimize)
		{
			WinStyle |= WS_MINIMIZEBOX;
		}
		if (WindowDesc->HasSizingBorder)
		{
			WinStyle |= WS_THICKFRAME;
		}

		SetWindowLong(Hwnd, GWL_STYLE, WinStyle);


		uint32 SetPositionFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;

		if (WindowDesc->ActivationPolicy == WindowActivation::Never)
		{
			SetPositionFlags |= SWP_NOACTIVATE;
		}


		SetWindowPos(Hwnd, nullptr, 0, 0, 0, 0, SetPositionFlags);
		AdjustWindowRegion(ClientWidth, ClientHeight);
		
	}
	else
	{
		if (!WindowDesc->HasCloseButton)
		{
			EnableMenuItem(GetSystemMenu(Hwnd, false), SC_CLOSE, MF_GRAYED);
		}
	}
	

	HasNeverShow = true;
	IsInitialized = true;

	return true;
}


void WindowsWindow::Reshape(int32 ClientX, int32 ClientY, int32 ClientWidth, int32 ClientHeight)
{
	WINDOWINFO WinInfo;
	Memory::Zero(&WinInfo, sizeof(WinInfo));
	WinInfo.cbSize = sizeof(WinInfo);
	::GetWindowInfo(Hwnd, &WinInfo);

	AspectRatio = (float)ClientWidth / (float)ClientHeight;

	int32 WindowX = ClientX;
	int32 WindowY = ClientY;
	int32 WindowWidth = ClientWidth;
	int32 WindowHeight = ClientHeight;

	/*
	//Keep the min width and height window size that keep allocated, to avoid resizing the associated buffer 
	const int32 MinMaintainedWidth = WindowDesc->DesiredMaxWidth > 0 ? WindowDesc->DesiredMaxWidth : ClientWidth;
	const int32 MinMaintainedHeight = WindowDesc->DesiredMaxHeight > 0 ? WindowDesc->DesiredMaxHeight : ClientHeight;

	WindowWidth = std::max(WindowWidth, std::min(ClientWidth, MinMaintainedWidth));
	WindowHeight = std::max(WindowHeight, std::min(ClientHeight, MinMaintainedHeight));
	*/

	//Get window rect with border
	if (WindowDesc->WinMode == WindowMode::Windowed)
	{
		RECT Rect = { 0, 0, 0, 0 };
		::AdjustWindowRectEx(&Rect, WinInfo.dwStyle, false, WinInfo.dwExStyle);

		//Rect.left and top are nagative
		WindowX += (WindowX + Rect.left >= 0  ? Rect.left : 0);
		WindowY += (WindowY + Rect.top >= 0 ? Rect.top : 0);
		WindowWidth += Rect.right - Rect.left;
		WindowHeight += Rect.bottom - Rect.top;
	}

	//bool ActualSizeChanged = WindowWidth != ActualWidth || WindowHeight != ActualHeight;
	//ActualWidth = WindowWidth;
	//ActualHeight = WindowHeight;

	if (IsMaximized())
	{
		Restore();
	}

	::SetWindowPos(Hwnd, nullptr, WindowX, WindowY, WindowWidth, WindowHeight, SWP_NOZORDER | SWP_NOACTIVATE);

	
	//Reset the window size to ActualWidth and ActualHeight
	/*
	if (ActualSizeChanged)
	{
		AdjustWindowRegion(ActualWidth, ActualHeight);
	}*/
}


void WindowsWindow::AdjustWindowRegion(int32 Width, int32 Height)
{
	
	HRGN Region ;
	if (IsMaximized())
	{
		if (WindowDesc->WinMode == WindowMode::Borderless)
		{
			WINDOWINFO WinInfo;
			Memory::Zero(&WinInfo, sizeof(WinInfo));
			WinInfo.cbSize = sizeof(WinInfo);
			::GetWindowInfo(Hwnd, &WinInfo);

			const int32 WindowBorderSize = WinInfo.cxWindowBorders;
			Region = CreateRectRgn(WindowBorderSize, WindowBorderSize, Width + WindowBorderSize, Height + WindowBorderSize);
		}
		else
		{
			const int32 BorderSize = GetBorderSize();
			Region = CreateRectRgn(BorderSize, BorderSize, Width - BorderSize, Height - BorderSize);
		}
	}
	else
	{
		//Region = CreateRoundRectRgn(0, 0, RegionWidth + 1, RegionHeight + 1, 0, 0);
		Region = CreateRectRgn(0, 0, Width, Height);
	}

	
	if (::SetWindowRgn(Hwnd, Region, false) == 0)
	{
		//log
	}
	
}


void WindowsWindow::Destroy()
{
	if (IsInitialized)
	{
		::DestroyWindow(Hwnd);
		Hwnd = NULL;
		IsInitialized = false;
		IsVisibled = false;
		HasNeverShow = false;
	}
}


void WindowsWindow::Show()
{
	if (!IsVisibled)
	{
		bool ShowWithActivated = false;
		if (WindowDesc->AcceptsInput)
		{
			ShowWithActivated = (WindowDesc->ActivationPolicy == WindowActivation::Always);
			if (HasNeverShow && WindowDesc->ActivationPolicy == WindowActivation::First)
			{
				ShowWithActivated = true;
			}
		}

		int CmdShow = ShowWithActivated ? SW_SHOW : SW_SHOWNOACTIVATE;
		if (HasNeverShow)
		{
			HasNeverShow = false;
			if (WindowDesc->InitiallyMaximized)
			{
				CmdShow = ShowWithActivated ? SW_SHOWMAXIMIZED : SW_MAXIMIZE;
			}
		}
		::ShowWindow(Hwnd, CmdShow);

		IsVisibled = true;
	}
}


void WindowsWindow::Hide()
{
	if (IsVisibled)
	{
		IsVisibled = false;
		::ShowWindow(Hwnd, SW_HIDE);
	}
}


void WindowsWindow::Restore()
{
	if (!HasNeverShow)
	{
		::ShowWindow(Hwnd, SW_RESTORE);
	}
}


void WindowsWindow::Focus()
{
	if (::GetFocus() != Hwnd)
	{
		::SetFocus(Hwnd);
	}
}


void WindowsWindow::Minimize()
{
	if (!HasNeverShow)
	{
		::ShowWindow(Hwnd, SW_MINIMIZE);
	}
}


void WindowsWindow::Maximize()
{
	if (!HasNeverShow)
	{
		::ShowWindow(Hwnd, SW_MAXIMIZE);
	}
}


void WindowsWindow::MoveWindowTo(int32 ClientX, int32 ClientY)
{
	int32 WindowX = ClientX;
	int32 WindowY = ClientY;

	if (WindowDesc->WinMode == WindowMode::Windowed)
	{
		const LONG WindowStyle = ::GetWindowLong(Hwnd, GWL_STYLE);
		const LONG WindowExStyle = ::GetWindowLong(Hwnd, GWL_EXSTYLE);

		//Get border size
		RECT Rect = { 0, 0, 0, 0 };
		::AdjustWindowRectEx(&Rect, WindowStyle, false, WindowExStyle);

		//Rect.left and right is nagative
		WindowX += Rect.left;
		WindowY += Rect.top;
	}

	::SetWindowPos(Hwnd, NULL, WindowX, WindowY, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}


int32 WindowsWindow::GetBorderSize()
{
	if (WindowDesc->WinMode == WindowMode::Borderless) return 0;

	WINDOWINFO WinInfo;
	Memory::Zero(&WinInfo, sizeof(WinInfo));
	WinInfo.cbSize = sizeof(WinInfo);
	::GetWindowInfo(Hwnd, &WinInfo);

	return WinInfo.cxWindowBorders;
}


void WindowsWindow::SetWindowMode(WindowMode NewMode)
{
	if (NewMode != WindowDesc->WinMode)
	{
		WindowDesc->WinMode = NewMode;
		
		LONG WindowStyle = GetWindowLong(Hwnd, GWL_STYLE);
		LONG WindowedModeStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;

		if (WindowDesc->SupportMaximize)
		{
			WindowedModeStyle |= WS_MAXIMIZEBOX;
		}

		if (WindowDesc->SupportMinimize)
		{
			WindowedModeStyle |= WS_MINIMIZEBOX;
		}

		if (WindowDesc->HasSizingBorder)
		{
			WindowedModeStyle |= WS_THICKFRAME;
		}
		else
		{
			WindowedModeStyle |= WS_BORDER;
		}

		if (NewMode == WindowMode::Windowed)
		{
			//Enable the non-client area rendering
			const DWMNCRENDERINGPOLICY Policy = DWMNCRP_ENABLED;
			if (FAILED(DwmSetWindowAttribute(Hwnd, DWMWA_NCRENDERING_POLICY, &Policy, sizeof(Policy))))
			{
				//log
			}
			//Enable content rendered in the non-client area
			const BOOL NCPaint = true;
			if (FAILED(DwmSetWindowAttribute(Hwnd, DWMWA_ALLOW_NCPAINT, &NCPaint, sizeof(NCPaint))))
			{
				//log
			}
			//Reset
			const MARGINS Margins = { 0 };
			if (FAILED(DwmExtendFrameIntoClientArea(Hwnd, &Margins)))
			{
				//log
			}


		}
		else if (NewMode == WindowMode::Borderless)
		{

			const DWMNCRENDERINGPOLICY Policy = DWMNCRP_DISABLED;
			if (FAILED(DwmSetWindowAttribute(Hwnd, DWMWA_NCRENDERING_POLICY, &Policy, sizeof(Policy))))
			{
				//log
			}

			const BOOL NCPaint = false;
			if (FAILED(DwmSetWindowAttribute(Hwnd, DWMWA_ALLOW_NCPAINT, &NCPaint, sizeof(NCPaint))))
			{
				//log
			}

			
			const MARGINS Margins = { -1 };
			if (FAILED(DwmExtendFrameIntoClientArea(Hwnd, &Margins)))
			{
				//log
			}
		}

		WindowStyle |= WindowedModeStyle;
		SetWindowLong(Hwnd, GWL_STYLE, WindowStyle);
		::SetWindowPos(Hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		UpdateWindow(Hwnd);
	}

}


void WindowsWindow::OnParentWindowMinimized()
{
	::GetWindowPlacement(Hwnd, &PreParentMinimizedWindowPlacement);
}


void WindowsWindow::OnParentWindowRestored()
{
	::SetWindowPlacement(Hwnd, &PreParentMinimizedWindowPlacement);
}


void WindowsWindow::GetClientSize(int32& OutWidth, int32& OutHeight)
{
	WINDOWINFO WinInfo;
	Memory::Zero(&WinInfo, sizeof(WinInfo));
	WinInfo.cbSize = sizeof(WinInfo);
	::GetWindowInfo(Hwnd, &WinInfo);

	OutWidth = WinInfo.rcClient.right - WinInfo.rcClient.left;
	OutHeight = WinInfo.rcClient.bottom - WinInfo.rcClient.top;

}

