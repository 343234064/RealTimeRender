#include "Editor/Editor.h"
#include "Global/Utilities/Misc.h"
#include "Global/EngineVariables.h"
#include "HAL/Time.h"





Editor* Editor::CreateEditor()
{
	if (gEditor == nullptr)
	{
		Editor* NewEditor = new Editor();
		return NewEditor;
	}
	else
	{
		return gEditor;
	}
		
}


Editor::Editor():
	PlatformEditorCore(nullptr),
	IsInitialized(false),
	CurrentTime(PlatformTime::Time_Seconds()),
	LastTickTime(0.0),
	AverageDeltaTime(1.0f/30.0f)
{
}


Editor::~Editor()
{
	if (IsInitialized)
	{
		Exit();
	}
	
}



bool Editor::Init()
{

	LoadEditorConfig();

	//key map init

	//Create Platform Editor
	if (!InitPlatformEditor())
	{
		return false;
	}
		

	//Create main window
	if (!InitMainWindow())
		return false;


	//Create GUI
	//if gui is init
	//set messager handler

	IsInitialized = true;
	return true;
}


void Editor::Exit()
{
	if (IsInitialized)
	{
		//gui shutdown

		
		PlatformEditorCore->Exit();

	
	    //reset array and pointer
		PlatformEditorCore.reset();

		IsInitialized = false;
	}

}


void Editor::Tick()
{
	LockGuard<PlatformCriticalSection> TickLock(TickCriticalSection);

	const float DeltaTime = (float)(CurrentTime - LastTickTime);

	PlatformEditorCore->Tick(DeltaTime);


	//OnPreTickEvent()

	ProcessDeferredCommand(DeltaTime);

	//render do release things



    //Update time and statistics
	LastTickTime = CurrentTime;
	CurrentTime = PlatformTime::Time_Seconds();

	const float AverageScale = 0.1f;
	AverageDeltaTime = AverageDeltaTime * (1.0f - AverageScale) + float(CurrentTime - LastTickTime) * AverageScale;

	/*
	//Calculate user idle time, if time > SleepThreshold, then IsUserIdle = true

	//If the frame rate is too low, the user idle time may not be avaliable, so add a TolerranceThreshold to this
	const double FrameTolerranceThreshold = 1.0 / 8.0;	// 8FPS
	if ((CurrentTime - LastTickTime) > FrameTolerranceThreshold)
	{
		LastTickTime = CurrentTime - FrameTolerranceThreshold;
	}

	const float SleepThreshold = GetSleepThreshold();
	const double TimeSinceInput = LastTickTime - LastUserInteractionTime;
	const double TimeSinceMouseMove = LastTickTime - LastMouseMoveTime;
	const bool IsUserIdle = (TimeSinceInput > SleepThreshold) && (TimeSinceMouseMove > SleepThreshold);
	
	*/

	//if IsUserIdle && main window ready && render ready
	//Render window gui and content
	//else
	//gShouldSleep = true

	//OnPostTickEvent()
}



bool Editor::InitPlatformEditor()
{
	PlatformEditor* NewPlatformEditor = nullptr;
#if PLATFORM_WINDOWS
	NewPlatformEditor = new WindowsEditor();
#elif PLATFORM_LINUX
#error No platform implement code here
#elif PLATFORM_MAC
#error No platform implement code here
#endif

	if (!NewPlatformEditor)
	{
		return false;
	}

	PlatformEditorCore.reset(NewPlatformEditor);

	return PlatformEditorCore->Init();
}


bool Editor::InitMainWindow()
{
	//get config of main window size and other stuff
	SharedPTRWinDescription MainWindowDesc(new WindowDescription());

	//Default settings
	MainWindowDesc->AcceptsInput = true;
	MainWindowDesc->ActivationPolicy = WindowActivation::Always;
	MainWindowDesc->AppearInTaskBar = true;
	MainWindowDesc->HasCloseButton = true;
	MainWindowDesc->HasSizingBorder = true;
	MainWindowDesc->KeepAspectRatio = true;
	MainWindowDesc->SupportMaximize = true;
	MainWindowDesc->SupportMinimize = true;
	MainWindowDesc->TopMostWindow = true;
	MainWindowDesc->Opacity = 1.0f;
	MainWindowDesc->Title = APPLICATION_NAME;
	MainWindowDesc->Title += APPLICATION_VERSION_NAME;

	//temp
	MainWindowDesc->DesiredPosXOnScreen = 100;
	MainWindowDesc->DesiredPosYOnScreen = 100;
	MainWindowDesc->DesiredWidthOnScreen = 1280;
	MainWindowDesc->DesiredHeightOnScreen = 720;
	MainWindowDesc->DesiredMaxHeight = 1080;
	MainWindowDesc->DesiredMaxWidth = 1920;
	MainWindowDesc->DesiredMinHeight = 600;
	MainWindowDesc->DesiredMinWidth = 800;
	MainWindowDesc->WinMode = WindowMode::Windowed;

	//Create
	return PlatformEditorCore->AddNewWindow(nullptr, MainWindowDesc, false, false);

}


void Editor::Show()
{
	//temp
	if (CanDisplayMainWindow() && PlatformEditorCore->GetWindowNum() >= 1)
	{
		PlatformEditorCore->ShowWindow(0);
		PlatformEditorCore->FocusWindow(0);
	}
}



void Editor::RequestCloseEditor()
{
    //Todo:Defer the call RequestCloseEditor() till next tick
	if (CanCloseEditor())
	{
		//Dothings that needed before finally exit
	    SaveEditorConfig();
		//Defercommand.add(CloseEditor)
		
		//Temp 
		CloseEditor();
    }
	
}


bool Editor::CanCloseEditor()
{
	//See if user is doing somthings on editor \ ask user to save the changed project
	//if so, stop it and call Defercommand.add(RequestCloseEditor) again
	//return false
	//else
	//return true

	return true;
}


bool Editor::CanDisplayMainWindow()
{
	//See if something is not finished before display main window
	//if so, stop it and call Defercommand.add(CanDisplayMainWindow) again
	//return false
	//else
	
	return true;
}

void Editor::CloseEditor()
{
	gIsAppRequestExit = true;
}




void Editor::LoadEditorConfig()
{
	//LoadGUIConfig
}


void Editor::SaveEditorConfig()
{

}


