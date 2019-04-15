/************************************
Gui


*************************************/
#pragma once

#include "Global/GlobalType.h"
#include "Editor/Window.h"



enum class MouseButton
{
	None = 0,
	Left = 1,
	Right = 2,
	Middle = 3
};

class ActionCallback
{
public:
	ActionCallback() {}
	virtual ~ActionCallback() {}

	virtual bool OnKeyChar(const TChar Character, const bool IsRepeat) { return false; }
	virtual bool OnKeyDown(const int32 KeyCode, const uint32 CharacterCode, const bool Repeat) { return false; }
	virtual bool OnKeyUp(const int32 KeyCode, const uint32 CharacterCode, const bool Repeat) { return false; }
	virtual bool OnMouseDown(const MouseButton Button, const float CursorScreenX, const float CursorScreenY) { return false; }
	virtual bool OnMouseUp(const MouseButton Button, const float CursorScreenX, const float CursorScreenY) { return false; }
	virtual bool OnMouseDoubleClick(const MouseButton Button, const float CursorScreenX, const float CursorScreenY) { return false; }
	virtual bool OnMouseWheel(const float Delta, const float CursorScreenX, const float CursorScreenY) { return false; }
	virtual bool OnMouseMove() { return false; }
	virtual bool OnWindowAction(const SharedPTRWindow& Window, const bool IsMaximized, const bool IsRestored) { return true; }
	virtual bool OnMoveWindow(const SharedPTRWindow& Window, const int32 X, const int32 Y) { return true; }
	virtual bool OnPaint(const SharedPTRWindow& Window) { return true; }
	virtual bool OnWindowActivationChanged(const SharedPTRWindow& Window, bool IsActivated, bool IsActivatedByMouse) { return true; }
	virtual bool OnWholeActivationChanged(const bool IsActivated) { return true; }
	virtual bool OnWindowClose(const SharedPTRWindow& Window) { return true; }
	virtual bool OnSizeChanged(const SharedPTRWindow& Window, const int32 NewWidth, const int32 NewHeight, bool bWasMinimized = false) { return true; }
	virtual bool OnResizingWindow(const SharedPTRWindow& Window) { return true; }
	virtual bool BeginResizingWindow(const SharedPTRWindow& Window) { return true; }
	virtual bool FinishResizingWindow(const SharedPTRWindow& Window) { return true; }


};
typedef std::shared_ptr<ActionCallback> SharedPTRCallback;