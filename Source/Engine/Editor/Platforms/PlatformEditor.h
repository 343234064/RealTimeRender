
/************************************
Platform dependent editor


*************************************/
#pragma once

#include "Editor/Window.h"
#include "Editor/ActionCallback.h"


#if PLATFORM_WINDOWS
#include "Editor/Platforms/Windows/WindowsMessage.h"
#elif PLATFORM_LINUX
#error No platform implement code here
#elif PLATFORM_MAC
#error No platform implement code here
#endif



class PlatformEditorInterface
{
public:
	PlatformEditorInterface() :
		Callback(nullptr),
		IsInitialized(false),
		IsDeferMessageProcessing(true)
	{}

	virtual ~PlatformEditorInterface()
	{
		if (IsInitialized)
		{
			//log error
		}
	}

	virtual bool Init() = 0;
	virtual void Tick(const float DeltaTime) = 0;
	virtual void Exit() = 0;

	virtual bool IsInit() { return IsInitialized; }

	virtual bool AddNewWindow(const SharedPTRWindow& ParentWindow, const SharedPTRWinDescription& Description, bool ShowImmediately, bool FocusImmediately) = 0;
	virtual void AddMessageHandler(PlatformMessageHandler* Handler) { MessageHandlers.AddUnique(Handler); }

	virtual void RemoveMessageHandler(PlatformMessageHandler* Handler) { MessageHandlers.Remove(Handler); }
	virtual void DestroySingleWindow(const SharedPTRWindow& ToDestroy);
	virtual void CloseAllWindowsImmediately();
	virtual void CloseSingleWindowImmediately(SharedPTRWindow& ToClose);

	virtual int32 GetWindowNum() { return Windows.CurrentNum(); }
	virtual int32 GetWindowIndex(void* WindowHandle);
	virtual const SharedPTRWindow& GetWindow(int32 WinIndex);

	virtual void  SetActionCallback(ActionCallback* CallBack) { Callback = CallBack; }

	virtual void  ProcessDeferredEvents(const float Delta);
	virtual int32 ProcessDeferredMessage(const PlatformDeferMessageArgs& Message) = 0;
	virtual void  PumpMessages(const float TimeDelta) = 0;

	virtual void ShowWindow(int32 WindowIndex) { Windows[WindowIndex]->Show(); }
	virtual void HideWindow(int32 WindowIndex) { Windows[WindowIndex]->Hide(); }
	virtual void FocusWindow(int32 WindowIndex) { Windows[WindowIndex]->Focus(); }


protected:
	//Windows[0] will be the main window, others are sub windows
	Array<SharedPTRWindow> Windows;

	//Allow adding message handler to handle msg you want
	//Raw pointer
	Array<PlatformMessageHandler*> MessageHandlers;

	//Window action callback
	//Raw pointer
	ActionCallback* Callback;


	//Messages are deferred-process until tick
	Array<PlatformDeferMessageArgs> DeferMessagesQueue;

	bool IsInitialized;

	//If message processing is deferred 
	bool IsDeferMessageProcessing;

};

