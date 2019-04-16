#include "Editor/Platforms/PlatformEditor.h"



int32 PlatformEditorInterface::GetWindowIndex(void* WindowHandle)
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


const SharedPTRWindow& PlatformEditorInterface::GetWindow(int32 WinIndex)
{
	//check
	return Windows[WinIndex];
}


void PlatformEditorInterface::CloseAllWindowsImmediately()
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


void PlatformEditorInterface::CloseSingleWindowImmediately(SharedPTRWindow& ToClose)
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


void PlatformEditorInterface::DestroySingleWindow(const SharedPTRWindow& ToDestroy)
{

	if (ToDestroy != nullptr)
	{
		ToDestroy->Destroy();
	}

}


void PlatformEditorInterface::ProcessDeferredEvents(const float Delta)
{
	//Make a copy to avoid processing the same messages twice 
	Array<PlatformDeferMessageArgs> MessageList(DeferMessagesQueue);
	DeferMessagesQueue.ClearElements();

	for (int32 Index = 0; Index < MessageList.CurrentNum(); Index++)
	{
		const PlatformDeferMessageArgs& Message = MessageList[Index];
		ProcessDeferredMessage(Message);
	}


}
