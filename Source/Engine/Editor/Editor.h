/************************************
Editor 


*************************************/
#pragma once


#include "Global/GlobalConfigs.h"
#include "Global/Utilities/DynamicArray.h" 
#include "HAL/Platform.h"

#if PLATFORM_WINDOWS
#include "Editor/Platforms/Windows/WindowsEditorMisc.h"
#elif PLATFORM_LINUX
#include "Editor/Platforms/Linux/LinuxEditorMisc.h"
#elif PLATFORM_MAC
#include "Editor/Platforms/Mac/MacEditorMisc.h"
#endif





class Editor
{
public:
	static Editor* CreateEditor();
	virtual ~Editor();

	Editor(Editor&&) = delete;
	Editor(const Editor&) = delete;
	Editor& operator=(Editor&&) = delete;
	Editor& operator=(const Editor&) = delete;
	


	bool Init();
	void Show();
	void Tick();
	void Exit();

	void RequestCloseEditor();

	bool IsInit() { return IsInitialized; }
	
	FORCE_INLINE
	PTRPlatformEditor& GetPlatformEditor() { return PlatformEditorCore; }




protected:
	Editor();
	
	bool InitPlatformEditor();
	bool InitMainWindow();

	bool CanDisplayMainWindow();

	bool CanCloseEditor();
	void CloseEditor();

	void LoadEditorConfig();
	void SaveEditorConfig();

	//Todo: deferred commands
	void  ProcessDeferredCommand(const float DeltaTime) {}



	

protected:
	//Platform Editor code
	PTRPlatformEditor PlatformEditorCore;

	//CriticalSection for tick event
	PlatformCriticalSection TickCriticalSection;

    //Todo: deferred commands
	//Array<Command> DeferredCommands;
	
	bool IsInitialized;

	//The current cached time
	double CurrentTime;

	//Last time that ticked
	double LastTickTime;

	//Running average time in seconds between calls to Tick, used for responsiveness
	float AverageDeltaTime;


};


extern Editor* gEditor;
