#include "Global/GlobalConfigs.h"

#if PLATFORM_WINDOWS


#include "Global/EngineVariables.h"
#include "Editor/Resources.h"
#include "Editor/Platforms/Windows/WindowsWindow.h"
#include "Editor/Platforms/Windows/WindowsEditorMisc.h"
#include "Editor/Editor.h"
#include "HAL/Path.h"


HICON GetMainWindowIcon()
{
	String IconPath = Path::Join(Path::GuiDir(), ICON_WINDOW_ADDRESS);

	return (HICON)LoadImage(nullptr, *IconPath, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
}


FORCE_INLINE
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return gEditor->GetPlatformEditor()->MainMessageHandler(hWnd, msg, wParam, lParam);
}


void RequestCloseEditor()
{
	gEditor->RequestCloseEditor();
}


bool WindowsEditor::Init()
{
	//Default callback to advoid null pointer crash
	Callback = this;

	//Disable the process from being showing "ghosted"(Not responding)
	::DisableProcessWindowsGhosting();

	//Register class
	WNDCLASS WC;
	Memory::Zero(&WC, sizeof(WC));

	HICON MainWinIcon = GetMainWindowIcon();
	if (MainWinIcon == NULL)
	{
		MainWinIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);
		//log load icon failed
	}

	WC.style = CS_DBLCLKS;
	WC.lpfnWndProc = WndProc;
	WC.cbClsExtra = 0;
	WC.cbWndExtra = 0;
	WC.hInstance = gMainInstanceHandle;
	WC.hIcon = MainWinIcon;
	WC.hCursor = NULL;
	WC.hbrBackground = NULL;
	WC.lpszMenuName = NULL;
	WC.lpszClassName = APPLICATION_IDENTITY;

	if (!::RegisterClass(&WC))
	{
		MessageBox(NULL, TEXT("Window Class Registration Failed"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
		//log
		return false;
	}

	IsInitialized = true;
	return true;
}


void WindowsEditor::Tick(const float DeltaTime)
{
	if (Windows.CurrentNum() > 0)
	{
		PumpMessages(DeltaTime);
	}

	ProcessDeferredEvents(DeltaTime);
}


void WindowsEditor::Exit()
{
	if (IsInitialized)
	{
		//Loop to destory all window Immediately
		CloseAllWindowsImmediately();

		//Flush all array
		if (DeferMessagesQueue.CurrentNum())
		{
			DeferMessagesQueue.Empty();
		}

		if (MessageHandlers.CurrentNum())
		{
			MessageHandlers.Empty();
		}

		//reset array and pointer
		
		//Callback will not be released here
		Callback = nullptr;
		
		IsActivateByMouse = false; 
		IsEnterSizeMove = false;
		
		IsInitialized = false;
	}
}




int32 WindowsEditor::MainMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	int32 SearchIndex = GetWindowIndex(hWnd);
	if (SearchIndex != -1)
	{
		SharedPTRWindow CurrentEventWindow = Windows[SearchIndex];

		//Editor user handlers handle messages
		int32 HandlerNum = MessageHandlers.CurrentNum();
		int32 ExternalHandledResult = 0;
		bool ExternalHandled = false;
		if (HandlerNum)
		{
			int32 HandledResult = 0;
			PlatformMessageHandler* Handler = nullptr;
			for (int32 Index = 0; Index < HandlerNum; Index++)
			{
				Handler = MessageHandlers[Index];
				if (Handler->ProcessMessage(hWnd, msg, wParam, lParam, HandledResult))
				{
					if (!ExternalHandled)
					{
						ExternalHandled = true;
						ExternalHandledResult = HandledResult;
					}
				}
			}
		}

		
		switch (msg)
		{
		case WM_INPUTLANGCHANGEREQUEST:
		case WM_INPUTLANGCHANGE:
		case WM_IME_SETCONTEXT:
		case WM_IME_STARTCOMPOSITION:
		case WM_IME_COMPOSITION:
		case WM_IME_ENDCOMPOSITION:
		case WM_IME_CHAR:
		case WM_IME_NOTIFY:
		case WM_IME_REQUEST:
		case WM_CHAR:
		{
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);
			return 0;
		}
		break;
		case WM_SYSCHAR:
		{
			//Alt+Space
			if ((HIWORD(lParam) & 0x2000) != 0 && wParam == VK_SPACE)
			{
				break;
			}
			else
			{
				return 0;
			}
		}
		break;
		case WM_SYSKEYDOWN:
		{
			if (wParam != VK_F4 && wParam != VK_SPACE)
			{
				DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);
			}
		}
		break;
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYUP:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_NCMOUSEMOVE:
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
		{
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);
			return 0;
		}
		break;
		case WM_INPUT:
		{
			//
		}
		break;

		//Sent when the size and position of a window's client area must be calculated. By processing this message, 
		//an application can control the content of the window's client area when the size or position of the window changes.
		case WM_NCCALCSIZE:
		{
			if (wParam && CurrentEventWindow->GetDescription()->WinMode == WindowMode::Borderless)
			{
				if (CurrentEventWindow->GetDescription()->WinMode == WindowMode::Borderless && CurrentEventWindow->IsMaximized())
				{
					//Borderless window will bleed the border out of the monitor when maximized
					//Here resize the borderless window to fit the monitor and send the result to WM_MOVE and WM_RESIZE
					WINDOWINFO WinInfo;
					Memory::Zero(&WinInfo, sizeof(WinInfo));
					WinInfo.cbSize = sizeof(WinInfo);
					::GetWindowInfo(hWnd, &WinInfo);

					LPNCCALCSIZE_PARAMS ResizeRects = (LPNCCALCSIZE_PARAMS)lParam;

					//New Window Rect
					ResizeRects->rgrc[0].left += WinInfo.cxWindowBorders;
					ResizeRects->rgrc[0].top += WinInfo.cxWindowBorders;
					ResizeRects->rgrc[0].right -= WinInfo.cxWindowBorders;
					ResizeRects->rgrc[0].bottom -= WinInfo.cxWindowBorders;

					//Dest Window Rect
					ResizeRects->rgrc[1].left = ResizeRects->rgrc[0].left;
					ResizeRects->rgrc[1].top = ResizeRects->rgrc[0].top;
					ResizeRects->rgrc[1].right = ResizeRects->rgrc[0].right;
					ResizeRects->rgrc[1].bottom = ResizeRects->rgrc[0].bottom;

					//Src Window Rect, Keep
					//ResizeRects->rgrc[2].left = ResizeRects->rgrc[2].left;
					//ResizeRects->rgrc[2].top = ResizeRects->rgrc[2].top;
					//ResizeRects->rgrc[2].right = ResizeRects->rgrc[2].right;
					//ResizeRects->rgrc[2].bottom = ResizeRects->rgrc[2].bottom;

					//New window pos
					ResizeRects->lppos->x += WinInfo.cxWindowBorders;
					ResizeRects->lppos->y += WinInfo.cxWindowBorders;
					ResizeRects->lppos->cx -= 2 * WinInfo.cxWindowBorders;//width
					ResizeRects->lppos->cy -= 2 * WinInfo.cxWindowBorders;//heigth

					//Use the changed value
					return WVR_VALIDRECTS;
				}
			}
		}
		break;
		case WM_SHOWWINDOW:
		{
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);
		}
		break;

		//Sent to a window AFTER its size has changed
		case WM_SIZE:
		{
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);

			bool Maximized = (wParam == SIZE_MAXIMIZED);
			bool Restored = (wParam == SIZE_MAXIMIZED);

			if (Maximized || Restored)
			{
				Callback->OnWindowAction(CurrentEventWindow, Maximized, Restored);
			}
			return 0;
		}
		break;

		//Sent to a window that the user is resizing
		case WM_SIZING:
		{
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);

			if (CurrentEventWindow->GetDescription()->KeepAspectRatio)
			{
				WINDOWINFO WinInfo;
				Memory::Zero(&WinInfo, sizeof(WinInfo));
				WinInfo.cbSize = sizeof(WinInfo);
				::GetWindowInfo(hWnd, &WinInfo);

				//Get title bar and border size
				RECT Rect = { 0, 0, 0, 0 };
				AdjustWindowRectEx(&Rect, WinInfo.dwStyle, false, WinInfo.dwExStyle);

				//Get the client height and width
				RECT* CurSize = (RECT*)lParam;
				CurSize->left -= Rect.left;
				CurSize->right -= Rect.right;
				CurSize->top -= Rect.top;
				CurSize->bottom -= Rect.bottom;

				int32 NewWidth = CurSize->right - CurSize->left;
				int32 NewHeight = CurSize->bottom - CurSize->top;

				int32 MinWidth = CurrentEventWindow->GetDescription()->DesiredMinWidth;
				int32 MinHeight = CurrentEventWindow->GetDescription()->DesiredMinHeight;

				const float AspectRatio = CurrentEventWindow->GetAspectRatio();

				switch (wParam)
				{
				case WMSZ_LEFT:
				case WMSZ_RIGHT:
				case WMSZ_BOTTOMLEFT:
				case WMSZ_BOTTOMRIGHT:
				case WMSZ_TOPLEFT:
				case WMSZ_TOPRIGHT:
				{
					if (MinHeight < MinWidth)
					{
						MinWidth = int32(MinHeight * AspectRatio);
					}
					if (NewWidth < MinWidth)
					{
						if (wParam == WMSZ_LEFT || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_TOPLEFT)
						{
							CurSize->left -= (MinWidth - NewWidth);
						}
						else if (wParam == WMSZ_RIGHT || wParam == WMSZ_BOTTOMRIGHT || wParam == WMSZ_TOPRIGHT)
						{
							CurSize->right += (MinWidth - NewWidth);
						}

						NewWidth = MinWidth;
					}
				}
				break;
				case WMSZ_TOP:
				case WMSZ_BOTTOM:
				{
					if (MinWidth < MinHeight)
					{
						MinHeight = int32(MinWidth / AspectRatio);
					}
					if (NewHeight < MinHeight)
					{
						if (wParam == WMSZ_TOP)
						{
							CurSize->top -= (MinHeight - NewHeight);
						}
						else
						{
							CurSize->bottom += (MinHeight - NewHeight);
						}

						NewHeight = MinHeight;
					}
				}
				break;
				default:
					break;
				}

				switch (wParam)
				{
				case WMSZ_LEFT:
				case WMSZ_RIGHT:
				{
					int32 AdjustedHeight = int32(NewWidth / AspectRatio);
					CurSize->top -= (AdjustedHeight - NewHeight) / 2;
					CurSize->bottom += (AdjustedHeight - NewHeight) / 2;
					break;
				}
				case WMSZ_TOP:
				case WMSZ_BOTTOM:
				{
					int32 AdjustedWidth = int32(NewHeight * AspectRatio);
					CurSize->left -= (AdjustedWidth - NewWidth) / 2;
					CurSize->right += (AdjustedWidth - NewWidth) / 2;
					break;
				}
				case WMSZ_TOPLEFT:
				{
					int32 AdjustedHeight = int32(NewWidth / AspectRatio);
					CurSize->top -= AdjustedHeight - NewHeight;
					break;
				}
				case WMSZ_TOPRIGHT:
				{
					int32 AdjustedHeight = int32(NewWidth / AspectRatio);
					CurSize->top -= AdjustedHeight - NewHeight;
					break;
				}
				case WMSZ_BOTTOMLEFT:
				{
					int32 AdjustedHeight = int32(NewWidth / AspectRatio);
					CurSize->bottom += AdjustedHeight - NewHeight;
					break;
				}
				case WMSZ_BOTTOMRIGHT:
				{
					int32 AdjustedHeight = int32(NewWidth / AspectRatio);
					CurSize->bottom += AdjustedHeight - NewHeight;
					break;
				}
				default:
					break;
				}

				AdjustWindowRectEx(CurSize, WinInfo.dwStyle, false, WinInfo.dwExStyle);
				return TRUE;
			}
		}
		break;
		case WM_ENTERSIZEMOVE:
		{
			IsEnterSizeMove = true;
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);
		}
		break;
		case WM_EXITSIZEMOVE:
		{
			IsEnterSizeMove = false;
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);
		}
		break;
		case WM_MOVE:
		{
			const int32 xPos = (int)(short)LOWORD(lParam);
			const int32 yPos = (int)(short)HIWORD(lParam);

			if (xPos > -32000 && yPos > -32000)
			{
				Callback->OnMoveWindow(CurrentEventWindow, xPos, yPos);
				return 0;
			}
		}
		break;
		case WM_NCHITTEST:
		{
			if (CurrentEventWindow->GetDescription()->WinMode == WindowMode::Borderless)
			{
				//
			}
		}
		break;
		case WM_MOUSEACTIVATE:
		case WM_ACTIVATE:
		{
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);
		}
		break;
		case WM_ACTIVATEAPP:
		{
			IsEnterSizeMove = false;
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);
		}
		break;
		case WM_PAINT:
		{
			if (IsEnterSizeMove)
			{
				Callback->OnPaint(CurrentEventWindow);
			}
		}
		break;
		case WM_ERASEBKGND:
		{
			//Return 1 to indicate that we'll handle the erasing, to eliminate flicker
			return 1;
		}
		break;
		case WM_NCACTIVATE:
		{
			//Unless using the window border, intercept calls to draw the non-client area a border upon activation or deactivation
			if (CurrentEventWindow->GetDescription()->WinMode == WindowMode::Borderless)
			{
				return true;
			}
		}
		break;
		case WM_NCPAINT:
		{
			//Unless using the window border, intercept calls to draw the non-client area(border)
			//This make the window look like no border, but the system button(min, max, close) still work fine
			if (CurrentEventWindow->GetDescription()->WinMode == WindowMode::Borderless)
			{
				return 0;
			}
		}
		break;
		case WM_DESTROY:
		{
			//Windows.RemoveAt(SearchIndex, 1, false);
			return 0;
		}
		break;
		case WM_CLOSE:
		{
			DeferMessage(CurrentEventWindow, hWnd, msg, wParam, lParam);
			return 0;
		}
		break;
		case WM_SYSCOMMAND:
		{
			switch (wParam & 0xfff0)
			{
			case SC_RESTORE:

				if (::IsIconic(hWnd))
				{
					::ShowWindow(hWnd, SW_RESTORE);
					return 0;
				}
				else
				{
					if (!Callback->OnWindowAction(CurrentEventWindow, false, true))
					{
						return 1;
					}
				}
				break;
			case SC_MAXIMIZE:
			{
				if (!Callback->OnWindowAction(CurrentEventWindow, true, false))
				{
					return 1;
				}
			}
			break;
			case SC_CLOSE:
			{
				DeferMessage(CurrentEventWindow, hWnd, WM_CLOSE, wParam, lParam);
				return 1;
			}
			break;
			default:
				break;
			}
		}
		break;

		//Sent to a window when the size or position of the window is about to change
		//An application can use this message to override the window's default maximized size and position, or its default minimum or maximum tracking size
		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* Info = (MINMAXINFO*)lParam;

			int32 MinWidth = CurrentEventWindow->GetDescription()->DesiredMinWidth;
			int32 MinHeight = CurrentEventWindow->GetDescription()->DesiredMinHeight;
			int32 MaxWidth = CurrentEventWindow->GetDescription()->DesiredMaxWidth;
			int32 MaxHeight = CurrentEventWindow->GetDescription()->DesiredMaxHeight;

			int32 BorderWidth = 0;
			int32 BorderHeight = 0;

			if (CurrentEventWindow->GetDescription()->WinMode == WindowMode::Windowed)
			{
				const DWORD WindowStyle = ::GetWindowLong(hWnd, GWL_STYLE);
				const DWORD WindowExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);

				RECT BorderRect = { 0, 0, 0, 0 };
				::AdjustWindowRectEx(&BorderRect, WindowStyle, false, WindowExStyle);

				BorderWidth = BorderRect.right - BorderRect.left;
				BorderHeight = BorderRect.bottom - BorderRect.top;
			}

			Info->ptMinTrackSize.x = MinWidth;
			Info->ptMinTrackSize.y = MinHeight;
			Info->ptMaxTrackSize.x = MaxWidth + BorderWidth;
			Info->ptMaxTrackSize.y = MaxHeight + BorderHeight;
		}
		break;
		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		{
			//
		}
		break;
		case WM_DISPLAYCHANGE:
		{
			//Monitor display resolution changed
		}
		break;
		case WM_CREATE:
			return 0;
		case WM_DEVICECHANGE:
		{
			//
		}
		break;
		default:
			if (ExternalHandled)
				return ExternalHandledResult;
			break;
		}
		
	}
	
	return (int32)DefWindowProc(hWnd, msg, wParam, lParam);
}


void WindowsEditor::DeferMessage(SharedPTRWindow& CurrentWindow, HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32 MouseX, int32 MouseY)
{
	if (gIsPumpingMessagesOutOfMainLoop && IsDeferMessageProcessing)
	{
		DeferMessagesQueue.Add(PlatformDeferMessageArgs(CurrentWindow, hWnd, msg, wParam, lParam, MouseX, MouseY));
	}
	else
	{
		ProcessDeferredMessage(PlatformDeferMessageArgs(CurrentWindow, hWnd, msg, wParam, lParam, MouseX, MouseY));
	}
}


int32 WindowsEditor::ProcessDeferredMessage(const PlatformDeferMessageArgs& Message)
{
	
	if (Windows.CurrentNum() && Message.CurrentWindow != nullptr)
	{
		HWND hWnd = Message.hWnd;
		uint32 msg = Message.msg;
		WPARAM wParam = Message.wParam;
		LPARAM lParam = Message.lParam;

		SharedPTRWindow CurrentWindow = Message.CurrentWindow;

		switch (msg)
		{
		case WM_INPUTLANGCHANGEREQUEST:
		case WM_INPUTLANGCHANGE:
		case WM_IME_SETCONTEXT:
		case WM_IME_NOTIFY:
		case WM_IME_REQUEST:
		case WM_IME_STARTCOMPOSITION:
		case WM_IME_COMPOSITION:
		case WM_IME_ENDCOMPOSITION:
		case WM_IME_CHAR:
		{
			//Unicode Text handle

		} break;
		case WM_CHAR:
		{
			const TChar Character = (TChar)wParam;
			const bool IsRepeat = (lParam & 0x40000000) != 0;
			Callback->OnKeyChar(Character, IsRepeat);

			return 0;
		} break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			int32 Key = (int32)wParam;
			const uint32 CharCode = ::MapVirtualKey(Key, MAPVK_VK_TO_CHAR);
			bool IsRepeat = (lParam & 0x40000000) != 0;
			
			switch (Key)
			{
			case VK_MENU://Alt
				if ((lParam & 0x1000000) == 0)
				{
					Key = VK_LMENU;
				}
				else
				{
					Key = VK_RMENU;
				}
				break;
			case VK_CONTROL://Control
				if ((lParam & 0x1000000) == 0)
				{
					Key = VK_LCONTROL;
				}
				else
				{
					Key = VK_RCONTROL;
				}
				break;
			case VK_SHIFT: //Shift
			{
				Key = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
			
			}	break;
			case VK_CAPITAL:
				break;
			default:
				break;
			}
			
				if (CurrentWindow != nullptr)
				{
					WindowMode Cur = CurrentWindow->GetWindowMode();
					if (Cur != WindowMode::Windowed)
						Cur = WindowMode::Windowed;
					else
						Cur = WindowMode::Borderless;
					CurrentWindow->SetWindowMode(Cur);
				}
			

			if (Callback->OnKeyDown(Key, CharCode, IsRepeat) || msg != WM_SYSKEYDOWN)
				return 0;

		} break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			int32 Key = (int32)wParam;
			const uint32 CharCode = ::MapVirtualKey(Key, MAPVK_VK_TO_CHAR);
			bool IsRepeat = (lParam & 0x40000000) != 0;

			switch (Key)
			{
			case VK_MENU://Alt
				if ((lParam & 0x1000000) == 0)
				{
					Key = VK_LMENU;
				}
				else
				{
					Key = VK_RMENU;
				}
				break;
			case VK_CONTROL://Control
				if ((lParam & 0x1000000) == 0)
				{
					Key = VK_LCONTROL;
				}
				else
				{
					Key = VK_RCONTROL;
				}
				break;
			case VK_SHIFT: //Shift
				Key = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
				break;
			case VK_CAPITAL:
				break;
			default:
				break;
			}

		
			if (Callback->OnKeyUp(Key, CharCode, IsRepeat) || msg != WM_SYSKEYUP)
				return 0;

		} break;
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_XBUTTONUP:
		{
			POINT Cursor;
			Cursor.x = (int)(short)LOWORD(lParam);
			Cursor.y = (int)(short)HIWORD(lParam);

			ClientToScreen(hWnd, &Cursor);

			bool DoubleClick = false;
			bool MouseUp = false;
			MouseButton Button = MouseButton::None;

			switch (msg)
			{
			case WM_LBUTTONDBLCLK:
				DoubleClick = true;
				Button = MouseButton::Left;
				break;
			case WM_LBUTTONUP:
				MouseUp = true;
				Button = MouseButton::Left;
				break;
			case WM_LBUTTONDOWN:
				Button = MouseButton::Left;
				break;
			case WM_MBUTTONDBLCLK:
				DoubleClick = true;
				Button = MouseButton::Middle;
				break;
			case WM_MBUTTONUP:
				MouseUp = true;
				Button = MouseButton::Middle;
				break;
			case WM_MBUTTONDOWN:
				Button = MouseButton::Middle;
				break;
			case WM_RBUTTONDBLCLK:
				DoubleClick = true;
				Button = MouseButton::Right;
				break;
			case WM_RBUTTONUP:
				MouseUp = true;
				Button = MouseButton::Right;
				break;
			case WM_RBUTTONDOWN:
				Button = MouseButton::Right;
				break;
			case WM_XBUTTONDBLCLK:
			case WM_XBUTTONUP:
			case WM_XBUTTONDOWN:
				break;
			default:
				break;
			}

			const float xPos = float(Cursor.x);
			const float yPos = float(Cursor.y);

			if (MouseUp)
				Callback->OnMouseUp(Button, xPos, yPos);
			else if(DoubleClick)
				Callback->OnMouseDoubleClick(Button, xPos, yPos);
			else
				Callback->OnMouseDown(Button, xPos, yPos);

			return 0;

		} break;
		case WM_INPUT:
		case WM_NCMOUSEMOVE:
		case WM_MOUSEMOVE:
		{
			return Callback->OnMouseMove() ? 0 : 1;

		} break;
		case WM_MOUSEWHEEL:
		{
			const float Factor = 0.0083333f;//1 / 120.0f;
			const SHORT WheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			POINT Cursor;
			Cursor.x = (int)(short)LOWORD(lParam);
			Cursor.y = (int)(short)HIWORD(lParam);

			const float xPos = float(Cursor.x);
			const float yPos = float(Cursor.y);

			return Callback->OnMouseWheel(float(WheelDelta)*Factor, xPos, yPos) ? 0 : 1;

		} break;
		case WM_MOUSEACTIVATE:
		{
			IsActivateByMouse = !(LOWORD(lParam) & HTCLIENT);
			return 0;
		} break;
		case WM_ACTIVATE:
		{
			bool Activated = false;
			bool IsMouseActivated = false;

			if (LOWORD(wParam) & WA_ACTIVE)
			{
				Activated = true;
				IsMouseActivated = IsActivateByMouse ? true : false;
			}
			else if (LOWORD(wParam) & WA_CLICKACTIVE)
			{
				Activated = true;
				IsMouseActivated = true;
			}

			IsActivateByMouse = false;

			if (CurrentWindow != nullptr)
			{
				return Callback->OnWindowActivationChanged(CurrentWindow, Activated, IsMouseActivated) ? 0 : 1;
			}
			return 1;
		}
		case WM_ACTIVATEAPP:
		{
			Callback->OnWholeActivationChanged(wParam == TRUE);
		} break;
		case WM_SETTINGCHANGE:
		{
			//
		} break;
		case WM_NCACTIVATE:
		{
			
			if (CurrentWindow != nullptr && CurrentWindow->GetDescription()->WinMode == WindowMode::Borderless)
			{
				return true;
			}
		} break;
		case WM_NCPAINT:
		{
			
			if (CurrentWindow != nullptr && CurrentWindow->GetDescription()->WinMode == WindowMode::Borderless)
			{
				return 0;
			}
		} break;
		case WM_CLOSE:
		{
			if (CurrentWindow != nullptr)
			{

				if (CurrentWindow != Windows[0])
				{
					Callback->OnWindowClose(CurrentWindow);
					//Close current window
					CloseSingleWindowImmediately(CurrentWindow);
				}
				else
				{
					//Close main window
					RequestCloseEditor();
				}
				  
			}
			return 0;
		} break;
		case WM_SHOWWINDOW:
		{
			if (CurrentWindow != nullptr)
			{
				switch (lParam)
				{
				case SW_PARENTCLOSING:
					CurrentWindow->OnParentWindowMinimized();
					break;
				case SW_PARENTOPENING:
					CurrentWindow->OnParentWindowRestored();
					break;
				default:
					break;
				}
			}
		} break;
		//Sent to a window AFTER its size has changed
		case WM_SIZE:
		{
			if (CurrentWindow != nullptr)
			{
				const int32 NewWidth = (int)(short)LOWORD(lParam);
				const int32 NewHeight = (int)(short)HIWORD(lParam);

				//Cut the region
				if (CurrentWindow->GetDescription()->WinMode == WindowMode::Borderless)
					CurrentWindow->AdjustWindowRegion(NewWidth, NewHeight);

				Callback->OnSizeChanged(CurrentWindow, NewWidth, NewHeight, wParam == SIZE_MINIMIZED);

			}
		} break;
		//Sent to a window AFTER its size has changed
		case WM_SIZING:
		{
			if (CurrentWindow != nullptr)
			{
				Callback->OnResizingWindow(CurrentWindow);
			}

		} break;
		case WM_ENTERSIZEMOVE:
		{
			if (CurrentWindow != nullptr)
			{
				Callback->BeginResizingWindow(CurrentWindow);
			}
		}
		break;
		case WM_EXITSIZEMOVE:
		{
			if (CurrentWindow != nullptr)
			{
				Callback->FinishResizingWindow(CurrentWindow);
			}
		}
		break;

		default:
			break;
		}
	}
	
	return 0;
}




bool WindowsEditor::AddNewWindow(const SharedPTRWindow& ParentWindow, const SharedPTRWinDescription& Description, bool ShowImmediately, bool FocusImmediately)
{

	SharedPTRWindow NewWindow(new WindowsWindow());

	if (NewWindow != nullptr && NewWindow->Create(Description, ParentWindow))
	{
		if (ShowImmediately)
		{
			NewWindow->Show();
			if (FocusImmediately)
			{
				NewWindow->Focus();
			}
		}

		Windows.Add(NewWindow);
	}
	else
	{
		MessageBox(NULL, TEXT("Window Create Failed"), TEXT("Error"), MB_ICONEXCLAMATION | MB_OK);
		//log
		return false;
	}

	return true;
	
}


void WindowsEditor::PumpMessages(const float TimeDelta)
{

	gIsPumpingMessagesOutOfMainLoop = false;
	MSG Message;

	while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	
	bool HasFocus = Platform::IsCurrentProcessForeground();
	static bool HadFocus = true;

	if (!HasFocus && HadFocus)
	{
		//Drag down the priority
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	}
	else if (HasFocus && !HadFocus)
	{
		//Boost up the priority
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}
	
	if (!HasFocus)
	{
		Platform::Sleep(0.005f);
	}
	HadFocus = HasFocus;
}





#endif