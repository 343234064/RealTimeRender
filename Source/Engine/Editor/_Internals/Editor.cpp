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
	Callback(nullptr),
	IsInitialized(false),
	IsDeferMessageProcessing(true),
	CurrentTime(PlatformTime::Time_Seconds()),
	LastTickTime(0.0),
	AverageDeltaTime(1.0f/30.0f)
{
	PLATFORM_EDITOR_MEMBER_INIT
}

Editor::~Editor()
{
	if (IsInitialized)
	{
		Exit();
	}
	
    PLATFORM_EDITOR_MEMBER_CLEANUP
}



bool Editor::Init()
{
	//set messager handler
	Callback = this;
	
	LoadEditorConfig();

	//key map init


	//Create main window
	if (!CreateMainWindow())
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


		IsInitialized = false;
	}

}


void Editor::Tick()
{
	LockGuard<PlatformCriticalSection> TickLock(TickCriticalSection);

	const float DeltaTime = (float)(CurrentTime - LastTickTime);

	if (Windows.CurrentNum() > 0)
	{
		PumpMessages(DeltaTime);
	}

	PlatformEditorTick();
	ProcessDeferredEvents(DeltaTime);

	//OnPreTickEvent()


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


void Editor::Show()
{
	//temp
	if (CanDisplayMainWindow() && Windows.CurrentNum() >= 1)
	{
		Windows[0]->Show();
		Windows[0]->Focus();
	}
}


int32 Editor::GetWindowIndex(void* WindowHandle)
{
	for (int32 Index = 0; Index < Windows.CurrentNum(); Index++)
	{
		SharedPTRWindow Window = Windows[Index];
		if (Window->GetWindowHandle() == WindowHandle)
		{
			return Index;
		}
	}

	return -1;
}


const SharedPTRWindow& Editor::GetWindow(int32 WinIndex)
{
	//check
	return Windows[WinIndex];
}


bool Editor::CanDisplayMainWindow()
{
	//Todo: see if gui renderer is ready
	return true;
}


void Editor::CloseAllWindowsImmediately()
{	
	//The application currently has at most 1 toplevel window and 1~2 subwindow
	//Destroy subwindow recursively is not needed

	while (Windows.CurrentNum() > 0)
	{
		int32 Index = Windows.CurrentNum() - 1;
		//Delete from top-down
		SharedPTRWindow CurrentWindow = Windows[Index];
		Windows.RemoveAt(Index, 1, false);

		DestroySingleWindow(CurrentWindow);

		CurrentWindow = nullptr;
	}

	Windows.Empty();
}


void Editor::CloseSingleWindowImmediately(SharedPTRWindow& ToClose)
{
	//The application currently has at most 1 toplevel window and 1~2 subwindow
	//Destroy subwindow recursively is not needed
	if (Windows.CurrentNum())
	{
		if (ToClose != Windows[0])
		{
			DestroySingleWindow(ToClose);
			Windows.Remove(ToClose);
			ToClose = nullptr;
		}
		else//Windows[0] is top window
		{
			//
		}
	}
	
}


void Editor::DestroySingleWindow(const SharedPTRWindow& ToDestroy)
{
	//Todo:Render release res..

	if (ToDestroy != nullptr)
	{
		ToDestroy->Destroy();
	}

}


void Editor::ProcessDeferredEvents(const float Delta)
{
	//Make a copy to avoid processing the same messages twice 
	Array<PlatformDeferMessageArgs> MessageList(DeferMessagesQueue);
	DeferMessagesQueue.ClearElements();

	for (int32 Index = 0; Index < MessageList.CurrentNum(); Index++)
	{
		const PlatformDeferMessageArgs& Message = MessageList[Index];
		ProcessDeferredMessage(Message);
	}


	//ProcessDeferredCommand
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