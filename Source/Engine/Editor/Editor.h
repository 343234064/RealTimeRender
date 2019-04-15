/************************************
Editor 


*************************************/
#pragma once


#include "Global/GlobalConfigs.h"
#include "Global/Utilities/String.h" 
#include "Global/Utilities/DynamicArray.h" 
#include "Editor/Window.h"
#include "Editor/GUI.h"
#include "HAL/Platform.h"

#if PLATFORM_WINDOWS
#include "Editor/Platforms/Windows/WindowsEditorMisc.h"
#elif PLATFORM_LINUX
#include "Editor/Platforms/Linux/LinuxEditorMisc.h"
#elif PLATFORM_MAC
#include "Editor/Platforms/Mac/MacEditorMisc.h"
#endif



#ifndef PLATFORM_EDITOR_PUBLIC_FUNCTIONS
#define PLATFORM_EDITOR_PUBLIC_FUNCTIONS
#endif
#ifndef PLATFORM_EDITOR_PROTECTED_FUNCTIONS
#define PLATFORM_EDITOR_PROTECTED_FUNCTIONS
#endif
#ifndef PLATFORM_EDITOR_PUBLIC_VARIABLES
#define PLATFORM_EDITOR_PUBLIC_VARIABLES
#endif
#ifndef PLATFORM_EDITOR_PROTECTED_VARIABLES
#define PLATFORM_EDITOR_PROTECTED_VARIABLES
#endif
#ifndef PLATFORM_EDITOR_MEMBER_INIT
#define PLATFORM_EDITOR_MEMBER_INIT
#endif
#ifndef PLATFORM_EDITOR_MEMBER_CLEANUP
#define PLATFORM_EDITOR_MEMBER_CLEANUP
#endif





class Editor:
	public ActionCallback
{
public:
	static Editor* CreateEditor();
	virtual ~Editor();

	bool Init();
	void Show();
	void Tick();
	void Exit();



	//Return index of match window
	int32 GetWindowIndex(void* WindowHandle);
	const SharedPTRWindow& GetWindow(int32 WinIndex);
	
	void AddMessageHandler(PlatformMessageHandler* Handler) { MessageHandlers.AddUnique(Handler); }
	void RemoveMessageHandler(PlatformMessageHandler* Handler) { MessageHandlers.Remove(Handler); }

	bool IsInit() { return IsInitialized; }

	void RequestCloseEditor();



	Editor(Editor&&) = delete;
	Editor(const Editor&) = delete;
	Editor& operator=(Editor&&) = delete;
	Editor& operator=(const Editor&) = delete;


protected:
	Editor();
	
	bool CreateMainWindow();
    void AddNewWindow(const SharedPTRWindow& ParentWindow, const SharedPTRWinDescription& Description, bool ShowImmediately, bool FocusImmediately);
	void DestroySingleWindow(const SharedPTRWindow& ToDestroy);
	void CloseAllWindowsImmediately();
	void CloseSingleWindowImmediately(SharedPTRWindow& ToClose);
	bool CanDisplayMainWindow();

	bool CanCloseEditor();
	void CloseEditor();

	void LoadEditorConfig();
	void SaveEditorConfig();

	void  ProcessDeferredEvents(const float Delta);
	int32 ProcessDeferredMessage(const PlatformDeferMessageArgs& Message);
	//Todo: deferred commands
    //void ProcessDeferredCommand(const Command& Cmd);

	void PlatformEditorTick();
    void PumpMessages(const float TimeDelta);

	



protected:
	//Windows[0] will be the main window, others are sub windows
	Array<SharedPTRWindow> Windows;
	
	//Allow adding message handler to handle msg you want
	//Raw pointer
	Array<PlatformMessageHandler*> MessageHandlers;

	//Window action callback
	//Raw pointer
	ActionCallback* Callback;

	//CriticalSection for tick event
	PlatformCriticalSection TickCriticalSection;

	//Messages are deferred-process until tick
	Array<PlatformDeferMessageArgs> DeferMessagesQueue; 

    //Todo: deferred commands
	//Array<Command> DeferredCommands;
	
	bool IsInitialized;

	//If message processing is deferred 
	bool IsDeferMessageProcessing; 

	//The current cached time
	double CurrentTime;

	//Last time that ticked
	double LastTickTime;

	//Running average time in seconds between calls to Tick, used for responsiveness
	float AverageDeltaTime;



/****Platform dependent members****/
public:

	PLATFORM_EDITOR_PUBLIC_FUNCTIONS

protected:

	PLATFORM_EDITOR_PROTECTED_FUNCTIONS

public:

	PLATFORM_EDITOR_PUBLIC_VARIABLES
	
protected:

	PLATFORM_EDITOR_PROTECTED_VARIABLES


};


extern Editor* gEditor;
