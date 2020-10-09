#pragma once

#include "HAL/Platforms/GenericProcess.h"
#include "HAL/Platforms/Windows/WindowsPlatform.h"


struct WindowsProcessHandle : public ProcessHandleBase<::HANDLE, nullptr>
{
public:
	FORCE_INLINE WindowsProcessHandle():
		ProcessHandleBase()
	{}

	FORCE_INLINE explicit WindowsProcessHandle(::HANDLE Other):
		ProcessHandleBase(Other)
	{}
};
typedef WindowsProcessHandle PlatformProcessHandle;


class WindowsProcess : public PlatformProcessBase
{
public:
	struct WindowsSemaphore: public PlatformSemaphore
	{
		WindowsSemaphore(const String& InName, ::HANDLE InSemaphore):
			PlatformSemaphore(InName),
			Semaphore(InSemaphore)
		{}

		WindowsSemaphore(const TChar* InName, ::HANDLE InSemaphore):
			PlatformSemaphore(InName),
			Semaphore(InSemaphore)
		{}

		virtual ~WindowsSemaphore()
		{
			// actual cleanup should be done in DeleteInterprocessSynchObject() since it can return errors
		}

		virtual void Lock();

		/**
		 * Nanoseconds : 10^-9 seconds
		 */
		virtual bool TryLock(uint64 NanosecondsToWait);

		virtual void UnLock();

		::HANDLE GetSemaphore() { return Semaphore; }

	protected:
		TChar Name[128];

		::HANDLE Semaphore;
		
	};

	static uint32 GetCurrentProcessId();

	static const TChar* CurrentExePath();

	static const TChar* CurrentExeName(bool WithExtension);

	/**
	* Creates a new process and its primary thread.
	* param URL					executable name
	* param Parms					command line arguments
	* param LaunchDetached		if true, the new process will have its own window
	* param LaunchHidden			if true, the new process will be minimized in the task bar
	* param LaunchReallyHidden	if true, the new process will not have a window or be in the task bar
	* param OutProcessId			if non-NULL, this will be filled in with the ProcessId
	* param PriorityModifier		-2 idle, -1 low, 0 normal, 1 high, 2 higher
	* param OptionalWorkingDirectory		Directory to start in when running the program, or NULL to use the current working directory
	* param PipeWrite				Optional HANDLE to pipe for redirecting output
	*/
	static PlatformProcessHandle CreateProc(const TChar* URL, const TChar* Parms, bool LaunchDetached, bool LaunchHidden, bool LaunchReallyHidden, uint32* OutProcessID, int32 PriorityModifier, const TChar* OptionalWorkingDirectory, void* PipeWriteChild, void* PipeReadChild = nullptr);

	static PlatformProcessHandle OpenProcess(uint32 ProcessID);

	static bool IsProcRunning(PlatformProcessHandle& ProcessHandle)
	{
		bool Running = true;
		uint32 WaitResult = ::WaitForSingleObject(ProcessHandle.Get(), 0);
		if (WaitResult != WAIT_TIMEOUT)
		{
			Running = false;
		}
		return Running;
	}

	static void WaitForProc(PlatformProcessHandle& ProcessHandle);

	static void CloseProc(PlatformProcessHandle& ProcessHandle)
	{
		if (ProcessHandle.IsValid())
		{
			::CloseHandle(ProcessHandle.Get());
			ProcessHandle.Reset();
		}
	}

	static void TerminateProc(PlatformProcessHandle& ProcessHandle, bool KillTree = false);

	static bool GetProcReturnCode(PlatformProcessHandle& ProcHandle, int32* ReturnCode)
	{
		DWORD ExitCode = 0;
		if (::GetExitCodeProcess(ProcHandle.Get(), &ExitCode) && ExitCode != STILL_ACTIVE)
		{
			if (ReturnCode != nullptr)
			{
				*ReturnCode = (int32)ExitCode;
			}
			return true;
		}
		return false;
	}

	static bool ExecProcess(const TChar* URL, const TChar* Params, int32* OutReturnCode, String* OutStdOut, String* OutStdErr, const TChar* OptionalWorkingDirectory = NULL);

	static void Sleep(float Seconds)
	{
		uint32 Milliseconds = (uint32)(Seconds * 1000.0);
		if (Milliseconds == 0)
		{
			::SwitchToThread();
		}
		else
		{
			::Sleep(Milliseconds);
		}
	}

	static void SleepInfinite()
	{
		::Sleep(INFINITE);
	}

	static void ClosePipe(void* ReadPipe, void* WritePipe);

	static bool CreatePipe(void*& ReadPipe, void*& WritePipe);

	static String ReadPipe(void* ReadPipe);

	static bool ReadPipeToArray(void* ReadPipe, Array<uint8>& Output);

	static bool WritePipe(void* WritePipe, const String& Message, String* OutWritten = nullptr);

	static bool WritePipe(void* WritePipe, const uint8* Data, const int32 DataLength, int32* OutDataLength = nullptr);

	static bool DeleteInterprocessSynchObject(PlatformSemaphore* Object);
};


typedef WindowsProcess PlatformProcess;