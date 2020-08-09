/************************************
Process relatives


*************************************/
#pragma once

#include "Global/GlobalType.h"
#include "Global/Utilities/String.h"
#include "HAL/Chars.h"
#include "Log/LogMacros.h"

template< typename T, T NullHandleValue>
struct ProcessHandleBase
{
public:
	ProcessHandleBase(): Handle(NullHandleValue)
	{}

	explicit ProcessHandleBase(T Other):
		Handle(Other)
	{}

	ProcessHandleBase& operator==(const ProcessHandleBase& Other)
	{
		if (this != &Other)
		{
			Handle = Other.Handle;
		}

		return *this;
	}

	T Get() const { return Handle; }

	void Reset() { Handle = NullHandleValue; }

	bool IsValid() const { return Handle != NullHandleValue; }


protected:
	T Handle;
};





struct GenericProcessHandle : public ProcessHandleBase<void*, nullptr>
{
public:
	FORCE_INLINE GenericProcessHandle() :
		ProcessHandleBase()
	{}

	FORCE_INLINE explicit GenericProcessHandle(::HANDLE Other) :
		ProcessHandleBase(Other)
	{}
};


class PlatformProcessBase
{
public:
	struct PlatformSemaphore
	{
		const TChar* GetName() const
		{
			return Name;
		}

		virtual void Lock() = 0;

		/**
		 * Nanoseconds : 10^-9 seconds
		 */
		virtual bool TryLock(uint64 NanosecondsToWait) = 0;

		virtual void UnLock() = 0;

		PlatformSemaphore(const String& InName)
		{
			PlatformChars::Strcpy(Name, sizeof(Name) - 1, *InName);
		}

		PlatformSemaphore(const TChar* InName)
		{
			PlatformChars::Strcpy(Name, sizeof(Name) - 1, InName);
		}

		virtual ~PlatformSemaphore() { };

	protected:
		TChar			Name[128];
	};

	static uint32 GetCurrentProcessId()
	{
		return 0;
	}

	static const TChar* CurrentExePath()
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::CurrentExePath not implemented"));
		return nullptr;
	}

	static const TChar* CurrentExeName(bool WithExtension)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::CurrentExeName not implemented"));
		return nullptr;
	}

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
	static GenericProcessHandle CreateProc(const TCHAR* URL, const TCHAR* Parms, bool LaunchDetached, bool LaunchHidden, bool LaunchReallyHidden, uint32* OutProcessID, int32 PriorityModifier, const TCHAR* OptionalWorkingDirectory, void* PipeWriteChild, void* PipeReadChild = nullptr)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::CreateProc not implemented"));
		return GenericProcessHandle();
	}

	/**
	* Opens an existing process.
	*
	* param ProcessID	The process id of the process for which we want to obtain a handle.
	*/
	static GenericProcessHandle OpenProcess(uint32 ProcessID)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::OpenProcess not implemented"));
		return GenericProcessHandle();
	}

	static bool IsProcRunning(GenericProcessHandle& ProcessHandle)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::IsProcRunning not implemented"));
		return false;
	}

	static void WaitForProc(GenericProcessHandle& ProcessHandle) 
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::WaitForProc not implemented"));
	}

	static void CloseProc(GenericProcessHandle& ProcessHandle) 
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::CloseProc not implemented"));
	}

	 /** 
	 * Terminates a process
	 * param KillTree Whether the entire process tree should be terminated.
	 */
	static void TerminateProc(GenericProcessHandle& ProcessHandle, bool KillTree = false) 
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::TerminateProc not implemented"));
	}

	enum class WaitAndForkResult : uint8
	{
		Error,
		Parent,
		Child
	};

	/**
	 * Waits for process signals and forks child processes.
	 *
	 * WaitAndFork stalls the invoking process and forks child processes when signals are sent to it from an external source.
	 * Forked child processes will provide a return value of WaitAndForkResult::Child, while the parent process
	 * will not return until IsEngineExitRequested() is true (WaitAndForkResult::Parent) or there was an error (WaitAndForkResult::Error)
	 * The signal the parent process expects is platform-specific (i.e. SIGRTMIN+1 on Linux).
	 */
	static WaitAndForkResult WaitAndFork() 
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::WaitAndFork not implemented"));
	}

	static bool GetProcReturnCode(GenericProcessHandle& ProcHandle, int32* ReturnCode)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::GetProcReturnCode not implemented"));
		return false;
	}

	/**
	 * Executes a process, returning the return code, stdout, and stderr. This
	 * call blocks until the process has returned.
	 * param OutReturnCode may be 0
	 * param OutStdOut may be 0
	 * param OutStdErr may be 0
	 * OptionalWorkingDirectory may be 0
	 */
	static bool ExecProcess(const TChar* URL, const TChar* Params, int32* OutReturnCode, String* OutStdOut, String* OutStdErr, const TChar* OptionalWorkingDirectory = NULL)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::ExecProcess not implemented"));
		return false;
	}


	/** Sleep this thread for Seconds.  0.0 means release the current time slice to let other threads get some attention. Uses stats.*/
	static void Sleep(float Seconds) 
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::Sleep not implemented"));
	}
	/** Sleep this thread for Seconds.  0.0 means release the current time slice to let other threads get some attention. */
	static void SleepNoStats(float Seconds) 
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::SleepNoStats not implemented"));
	}
	/** Sleep this thread infinitely. */
	static void SleepInfinite() 
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::SleepInfinite not implemented"));
	}

	/**
	* Closes an anonymous pipe.
	*
	* param ReadPipe The handle to the read end of the pipe.
	* param WritePipe The handle to the write end of the pipe.
	*/
	static void ClosePipe(void* ReadPipe, void* WritePipe) 
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::ClosePipe not implemented"));
	}

	/**
	* Creates a writable anonymous pipe.
	*
	* Anonymous pipes can be used to capture and/or redirect STDOUT and STDERROR of a process.
	* The pipe created by this method can be passed into CreateProc as Write
	*
	* param ReadPipe Will hold the handle to the read end of the pipe.
	* param WritePipe Will hold the handle to the write end of the pipe.
	*/
	static bool CreatePipe(void*& ReadPipe, void*& WritePipe) 
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::CreatePipe not implemented"));
		return false;
	}

	 /**
	 * Reads all pending data from an anonymous pipe, such as STDOUT or STDERROR of a process.
	 *
	 * param Pipe The handle to the pipe to read from.
	 * return A string containing the read data.
	 */
	static String ReadPipe(void* ReadPipe)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::ReadPipe not implemented"));
		return String();
	}

	/**
	* Reads all pending data from an anonymous pipe, such as STDOUT or STDERROR of a process.
	 *
	* param Pipe The handle to the pipe to read from.
	* param Output The data read.
	* return true if successful (i.e. any data was read)
	*/
	static bool ReadPipeToArray(void* ReadPipe, Array<uint8>& Output)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::ReadPipeToArray not implemented"));
		return false;
	}

	/**
	* Sends the message to process through pipe
	*
	* param WritePipe Pipe for writing.
	* param Message The message to be written.
	* param OutWritten Optional parameter to know how much of the string written.
	* return True if all bytes written successfully.
	*/
	static bool WritePipe(void* WritePipe, const String& Message, String* OutWritten = nullptr)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::WritePipe not implemented"));
		return false;
	}

	/**
	* Sends data to process through pipe
	*
	* param WritePipe Pipe for writing.
	* param Data The data to be written.
	* param DataLength how many bytes to write.
	* param OutDataLength Optional parameter to know how many bytes had been written.
	* return True if all bytes written successfully.
	*/
	static bool WritePipe(void* WritePipe, const uint8* Data, const int32 DataLength, int32* OutDataLength = nullptr)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::WritePipe not implemented"));
		return false;
	}

	/**
	 * Deletes an interprocess synchronization object.
	 *
	 * @param Object object to destroy.
	 */
	static bool DeleteInterprocessSynchObject(PlatformSemaphore* Object)
	{
		LOG(Fatal, PlatformProcessBase, TEXTS("PlatformProcessBase::DeleteInterprocessSynchObject not implemented"));
		return false;
	}
};