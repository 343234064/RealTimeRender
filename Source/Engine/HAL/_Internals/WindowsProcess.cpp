#include "Global/Utilities/Assertion.h"
#include "Global/Utilities/CharConversion.h"
#include "Log/LogMacros.h"
#include "HAL/Platforms/Windows/WindowsProcess.h"
#include "HAL/Path.h"



#include <TlHelp32.h>

void WindowsProcess::WindowsSemaphore::Lock()
{
	CHECK(Semaphore != nullptr);

	DWORD WaitResult = ::WaitForSingleObject(Semaphore, INFINITE);
	if (WaitResult != WAIT_OBJECT_0)
	{
		DWORD Error = ::GetLastError();
		LOG(Warn, WindowsSemaphore, TEXTS("WaitForSingleObject(,INFINITE) for semaphore '%s' failed with return code 0x%08x and LastError = %d"), GetName(), WaitResult, Error);
	}
}

bool WindowsProcess::WindowsSemaphore::TryLock(uint64 NanosecondsToWait)
{
	CHECK(Semaphore != nullptr);

	DWORD MillisecondsToWait = (DWORD)(NanosecondsToWait / 1000000ULL);
	DWORD WaitResult = ::WaitForSingleObject(Semaphore, MillisecondsToWait);

	if (WaitResult != WAIT_OBJECT_0 && WaitResult != WAIT_TIMEOUT)	// timeout is not a warning
	{
		DWORD Error = ::GetLastError();
		LOG(Warn, WindowsSemaphore, TEXTS("WaitForSingleObject(,INFINITE) for semaphore '%s' failed with return code 0x%08x and LastError = %d"), GetName(), WaitResult, Error);
	}

	return WaitResult == WAIT_OBJECT_0;
}

void WindowsProcess::WindowsSemaphore::UnLock()
{
	CHECK(Semaphore != nullptr);

	if (!::ReleaseSemaphore(Semaphore, 1, NULL))
	{
		DWORD Error = GetLastError();
		LOG(Warn, WindowsSemaphore, TEXT("ReleaseSemaphore(,ReleaseCount=1,) for semaphore '%s' failed with LastError = %d"), GetName(), Error);
	}
}


uint32 WindowsProcess::GetCurrentProcessId()
{
	return ::GetCurrentProcessId();
}

const TChar* WindowsProcess::CurrentExePath()
{
	static TChar Path[512] = TEXTS("");
	if (!Path[0])
	{
		if (!::GetModuleFileName(gMainInstanceHandle, Path, (DWORD)512))
		{
			Path[0] = 0;
		}
	}

	return Path;
}

const TChar* WindowsProcess::CurrentExeName(bool WithExtension)
{
	static TChar Name[512] = TEXTS("");
	static TChar NameWithExt[512] = TEXTS("");

	if (!Name[0])
	{
		if (::GetModuleFileName(gMainInstanceHandle, Name, (DWORD)512) != 0)
		{
			String FileName = Name;
			String FileNameWithExt = Name;
			PlatformChars::Strncpy(Name, *(Path::GetFileName(FileName, false)), 512);
			PlatformChars::Strncpy(NameWithExt, *(Path::GetFileName(FileNameWithExt, true)), 512);
		}
		else
		{
			Memory::Zero(Name, 512);
			Memory::Zero(NameWithExt, 512);
		}
	}

	return (WithExtension ? NameWithExt : Name);
}

PlatformProcessHandle WindowsProcess::CreateProc(const TChar* URL, const TChar* Parms, bool LaunchDetached, bool LaunchHidden, bool LaunchReallyHidden, uint32* OutProcessID, int32 PriorityModifier, const TChar* OptionalWorkingDirectory, void* PipeWriteChild, void* PipeReadChild)
{
	LOG(Info, WindowsProcess, TEXTS("Create Process URL: %s,  Parms: %s"), URL, Parms);

	::SECURITY_ATTRIBUTES Attr;
	Attr.nLength = sizeof(::SECURITY_ATTRIBUTES);
	Attr.lpSecurityDescriptor = NULL;
	Attr.bInheritHandle = true;

	uint32 CreateFlags = NORMAL_PRIORITY_CLASS;
	if (PriorityModifier < 0)
	{
		CreateFlags = (PriorityModifier == -1) ? BELOW_NORMAL_PRIORITY_CLASS : IDLE_PRIORITY_CLASS;
	}
	else if (PriorityModifier > 0)
	{
		CreateFlags = (PriorityModifier == 1) ? ABOVE_NORMAL_PRIORITY_CLASS : HIGH_PRIORITY_CLASS;
	}
	
	if (LaunchDetached)
	{
		CreateFlags |= DETACHED_PROCESS;
	}

	DWORD DwFlags = 0;
	uint16 ShowWindowFlags = SW_HIDE;
	if (LaunchReallyHidden)
	{
		DwFlags = STARTF_USESHOWWINDOW;
	}
	else if (LaunchHidden)
	{
		DwFlags = STARTF_USESHOWWINDOW;
		ShowWindowFlags = SW_SHOWMINNOACTIVE;
	}

	if (PipeWriteChild != nullptr || PipeReadChild != nullptr)
	{
		DwFlags |= STARTF_USESTDHANDLES;
	}

	::STARTUPINFO StartUpInfo = {
		sizeof(::STARTUPINFO),
		NULL,
		NULL,
		NULL,
		(DWORD)CW_USEDEFAULT,
		(DWORD)CW_USEDEFAULT,
		(DWORD)CW_USEDEFAULT,
		(DWORD)CW_USEDEFAULT,
		(DWORD)0, (DWORD)0, (DWORD)0,
		DwFlags,
		ShowWindowFlags,
		0, NULL,
		::HANDLE(PipeReadChild),
		::HANDLE(PipeWriteChild),
		::HANDLE(PipeWriteChild)
	};

	String Command = String::Sprintf(TEXTS("\"%s\" %s"), URL, Parms);
	::PROCESS_INFORMATION ProcInfo;

	if (!::CreateProcess(NULL, *Command, &Attr, &Attr, true, (DWORD)CreateFlags, NULL, OptionalWorkingDirectory, &StartUpInfo, &ProcInfo))
	{
		DWORD ErrorCode = ::GetLastError();

		TChar ErrorMessage[512];
		Platform::GetSystemErrorMessage(ErrorMessage, 512, ErrorCode);

		LOG(Error, WindowsProcess, TEXTS("Create Process Failed, ErrorCode: (0x%08x), %s"), ErrorCode, ErrorMessage);
		if (ErrorCode == ERROR_NOT_ENOUGH_MEMORY || ErrorCode == ERROR_OUTOFMEMORY)
		{
			PlatformMemoryStates Stats = PlatformMemory::GetMemoryState();
			LOG(Error, WindowsProcess, TEXTS("Mem used: %.2f MB, OS Free %.2f MB"), Stats.PhysicalMemoryUsed / 1048576.0f, Stats.PhysicalMemoryAvailable / 1048576.0f);
		}

		if (OutProcessID != nullptr)
		{
			*OutProcessID = 0;
		}

		return PlatformProcessHandle();
	}

	if (OutProcessID != nullptr)
	{
		*OutProcessID = ProcInfo.dwProcessId;
	}

	::CloseHandle(ProcInfo.hThread);

	return PlatformProcessHandle(ProcInfo.hProcess);
}


PlatformProcessHandle WindowsProcess::OpenProcess(uint32 ProcessID)
{
	return PlatformProcessHandle(::OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessID));
}



void WindowsProcess::WaitForProc(PlatformProcessHandle& ProcessHandle)
{
	::WaitForSingleObject(ProcessHandle.Get(), INFINITE);
}


void WindowsProcess::TerminateProc(PlatformProcessHandle& ProcessHandle, bool KillTree)
{
	if (KillTree)
	{
		::HANDLE SnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (SnapShot != INVALID_HANDLE_VALUE)
		{
			DWORD ProcessId = ::GetProcessId(ProcessHandle.Get());

			::PROCESSENTRY32 Entry;
			Entry.dwSize = sizeof(::PROCESSENTRY32);

			if (::Process32First(SnapShot, &Entry))
			{
				do
				{
					if (Entry.th32ParentProcessID == ProcessId)
					{
						::HANDLE ChildProcHandle = ::OpenProcess(PROCESS_ALL_ACCESS, 0, Entry.th32ParentProcessID);

						if (ChildProcHandle)
						{
							PlatformProcessHandle ChildHandle(ChildProcHandle);
							TerminateProc(ChildHandle, KillTree);
						}
					}
				} while (::Process32Next(SnapShot, &Entry));
			}
		}
	}

	::TerminateProcess(ProcessHandle.Get(), 0);
}



bool WindowsProcess::ExecProcess(const TChar* URL, const TChar* Params, int32* OutReturnCode, String* OutStdOut, String* OutStdErr, const TChar* OptionalWorkingDirectory)
{
	::STARTUPINFOEX StartUpInfo;
	::ZeroMemory(&StartUpInfo, sizeof(StartUpInfo));
	StartUpInfo.StartupInfo.cb = sizeof(StartUpInfo);
	StartUpInfo.StartupInfo.dwX = (DWORD)CW_USEDEFAULT;
	StartUpInfo.StartupInfo.dwY = (DWORD)CW_USEDEFAULT;
	StartUpInfo.StartupInfo.dwXSize = (DWORD)CW_USEDEFAULT;
	StartUpInfo.StartupInfo.dwYSize = (DWORD)CW_USEDEFAULT;
	StartUpInfo.StartupInfo.dwFlags = (DWORD)STARTF_USESHOWWINDOW;
	StartUpInfo.StartupInfo.wShowWindow = (DWORD)SW_SHOWMINNOACTIVE;
	StartUpInfo.StartupInfo.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);

	::HANDLE StdOutRead = NULL;
	::HANDLE StdErrRead = NULL;
	Array<uint8> AttributeList;

	if (OutStdOut != nullptr || OutStdErr != nullptr)
	{
		StartUpInfo.StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

		::SECURITY_ATTRIBUTES Attr;
		::ZeroMemory(&Attr, sizeof(Attr));
		Attr.nLength = sizeof(::SECURITY_ATTRIBUTES);
		Attr.bInheritHandle = TRUE;

		bool Result = ::CreatePipe(&StdOutRead, &StartUpInfo.StartupInfo.hStdOutput, &Attr, 0);
		CHECK(Result);
		Result = ::CreatePipe(&StdErrRead, &StartUpInfo.StartupInfo.hStdError, &Attr, 0);
		CHECK(Result);

		SIZE_T BufferSize = 0;
		if (!::InitializeProcThreadAttributeList(NULL, 1, 0, &BufferSize) && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			AttributeList.AddUninitialize((int32)BufferSize);
			StartUpInfo.lpAttributeList = (::LPPROC_THREAD_ATTRIBUTE_LIST)AttributeList.Begin();
			Result = ::InitializeProcThreadAttributeList(StartUpInfo.lpAttributeList, 1, 0, &BufferSize);
			CHECK(Result);
		}

		::HANDLE StdHandles[2] = { StartUpInfo.StartupInfo.hStdOutput, StartUpInfo.StartupInfo.hStdError };
		Result = ::UpdateProcThreadAttribute(StartUpInfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, StdHandles, sizeof(StdHandles), NULL, NULL);
		CHECK(Result);
	}

	bool Success = false;
	String Command;
	if (URL[0] != '\"')
	{
		Command = String::Sprintf(TEXTS("\"%s\" %s"), URL, Params);
	}
	else
	{
		Command = String::Sprintf(TEXTS("%s %s"), URL, Params);
	}

	uint32 CreateFlags = NORMAL_PRIORITY_CLASS | DETACHED_PROCESS;
	if (StartUpInfo.lpAttributeList != NULL)
	{
		CreateFlags = EXTENDED_STARTUPINFO_PRESENT;
	}

	::PROCESS_INFORMATION ProcInfo;
	if (::CreateProcess(NULL, *Command, NULL, NULL, TRUE, CreateFlags, NULL, OptionalWorkingDirectory, &StartUpInfo.StartupInfo, &ProcInfo))
	{
		if (StdOutRead != NULL)
		{
			::HANDLE ReadablePipes[2] = { StdOutRead, StdErrRead };
			String* OutStrings[2] = { OutStdOut , OutStdErr };
			Array<uint8> PipeBytes[2];

			auto ReadPipes = [&]()
			{
				for (int32 PipeIndex = 0; PipeIndex < 2; ++PipeIndex)
				{
					if (ReadablePipes[PipeIndex] && OutStrings[PipeIndex])
					{
						Array<uint8> BinaryData;
						WindowsProcess::ReadPipeToArray(ReadablePipes[PipeIndex], BinaryData);
						PipeBytes[PipeIndex].Append(BinaryData);
					}
				}
			};

			PlatformProcessHandle ProcHandle(ProcInfo.hProcess);
			do
			{
				ReadPipes();
				WindowsProcess::Sleep(0);
			} while (WindowsProcess::IsProcRunning(ProcHandle));
			ReadPipes();

			for (int32 PipeIndex = 0; PipeIndex < 2; ++PipeIndex)
			{
				if (OutStrings[PipeIndex] && PipeBytes[PipeIndex].CurrentNum() > 0)
				{
					PipeBytes[PipeIndex].Add('\0');

					const ANSICHAR* PipeChars = (const ANSICHAR*)PipeBytes[PipeIndex].Begin();
					Array<TChar> ConvertedChars;
					UTF8ToTChar::Convert(ConvertedChars, PipeChars);

					*OutStrings[PipeIndex] = ConvertedChars.Begin();
				}
			}

		}
		else
		{
			::WaitForSingleObject(ProcInfo.hProcess, INFINITE);
		}

		if (OutReturnCode)
		{
			bool Result = ::GetExitCodeProcess(ProcInfo.hProcess, (DWORD*)OutReturnCode);
			CHECK(Result);
		}
		::CloseHandle(ProcInfo.hProcess);
		::CloseHandle(ProcInfo.hThread);
		Success = true;
	}
	else
	{
		DWORD ErrorCode = ::GetLastError();

		if (OutReturnCode)
		{
			*OutReturnCode = ErrorCode;
		}

		TChar ErrorMessage[512];
		Platform::GetSystemErrorMessage(ErrorMessage, 512, ErrorCode);
		LOG(Error, WindowsProcess, TEXTS("Create Process Failed, ErrorCode: (0x%08x), %s"), ErrorCode, ErrorMessage);
		if (ErrorCode == ERROR_NOT_ENOUGH_MEMORY || ErrorCode == ERROR_OUTOFMEMORY)
		{
			PlatformMemoryStates Stats = PlatformMemory::GetMemoryState();
			LOG(Error, WindowsProcess, TEXTS("Mem used: %.2f MB, OS Free %.2f MB"), Stats.PhysicalMemoryUsed / 1048576.0f, Stats.PhysicalMemoryAvailable / 1048576.0f);
		}
		LOG(Error, WindowsProcess, TEXTS("URL: %s %s"), URL, Params);
	}

	if (StartUpInfo.StartupInfo.hStdOutput != NULL)
	{
		::CloseHandle(StartUpInfo.StartupInfo.hStdOutput);
	}
	if (StartUpInfo.StartupInfo.hStdError!= NULL)
	{
		::CloseHandle(StartUpInfo.StartupInfo.hStdError);
	}

	if (StdOutRead != NULL)
	{
		::CloseHandle(StdOutRead);
	}
	if (StdErrRead != NULL)
	{
		::CloseHandle(StdErrRead);
	}

	if (StartUpInfo.lpAttributeList != NULL)
	{
		::DeleteProcThreadAttributeList(StartUpInfo.lpAttributeList);
	}

	return Success;
}


void WindowsProcess::ClosePipe(void* ReadPipe, void* WritePipe)
{
	if (ReadPipe != nullptr && ReadPipe != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(ReadPipe);
	}
	if (WritePipe != nullptr && WritePipe != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(WritePipe);
	}
}

bool WindowsProcess::CreatePipe(void*& ReadPipe, void*& WritePipe)
{
	::SECURITY_ATTRIBUTES Attr = { sizeof(::SECURITY_ATTRIBUTES), NULL, true };
	
	if (!::CreatePipe(&ReadPipe, &WritePipe, &Attr, 0))
	{
		return false;
	}

	if (!::SetHandleInformation(ReadPipe, HANDLE_FLAG_INHERIT, 0))
	{
		return false;
	}

	return true;
}

String WindowsProcess::ReadPipe(void* ReadPipe)
{
	String Output;

	uint32 BytesAvailable = 0;
	if (::PeekNamedPipe(ReadPipe, NULL, 0, NULL, (DWORD*)&BytesAvailable, NULL) && (BytesAvailable > 0))
	{
		ANSICHAR* Buffer = new ANSICHAR[BytesAvailable + 1];
		uint32 BytesRead = 0;
		if (::ReadFile(ReadPipe, Buffer, BytesAvailable, (DWORD*)&BytesRead, NULL))
		{
			if (BytesRead > 0)
			{
				Buffer[BytesRead] = '\0';
				Array<TChar> ConvertedChars;
				UTF8ToTChar::Convert(ConvertedChars, (const ANSICHAR*)Buffer);
				Output += ConvertedChars.Begin();
			}
		}
		delete[] Buffer;
	}

	return Output;
}

bool WindowsProcess::ReadPipeToArray(void* ReadPipe, Array<uint8>& Output)
{
	uint32 BytesAvailable = 0;
	if (::PeekNamedPipe(ReadPipe, NULL, 0, NULL, (DWORD*)&BytesAvailable, NULL) && (BytesAvailable > 0))
	{
		Output.AddUninitialize(BytesAvailable);
		uint32 BytesRead = 0;
		if (::ReadFile(ReadPipe, Output.Begin(), BytesAvailable, (DWORD*)&BytesRead, NULL))
		{
			if (BytesRead < BytesAvailable)
			{
				Output.SetNum(BytesRead);
			}

			return true;
		}
		else
		{
			Output.ClearElements();
		}
	}

	return false;
}

bool WindowsProcess::WritePipe(void* WritePipe, const String& Message, String* OutWritten)
{
	if (Message.Len() == 0 || WritePipe == nullptr)
	{
		return false;
	}

	Array<ANSICHAR> ConvertedANSIChars;
	TCharToUTF8::Convert(ConvertedANSIChars, *Message);

	uint32 BytesAvailable = ConvertedANSIChars.CurrentNum();
	ANSICHAR* Buffer = new ANSICHAR[BytesAvailable + 2];

	for (uint32 i = 0 ; i < BytesAvailable ; i++)
	{
		Buffer[i] = ConvertedANSIChars[i];
	}
	if (Buffer[BytesAvailable - 1] == '\0')
	{
		Buffer[BytesAvailable - 1] = '\n';
	}
	else
	{ 
		Buffer[BytesAvailable] = '\n';
	}

	uint32 BytesWritten = 0;
	bool IsWritten = !!::WriteFile(WritePipe, Buffer, BytesAvailable + 1, (::DWORD*) & BytesWritten, nullptr);


	if (OutWritten)
	{
		Buffer[BytesWritten] = '\0';
		Array<TChar> ConvertedChars;
		UTF8ToTChar::Convert(ConvertedChars, (const ANSICHAR*)Buffer);
		*OutWritten = ConvertedChars.Begin();
	}

	delete[] Buffer;
	return IsWritten;

}

bool WindowsProcess::WritePipe(void* WritePipe, const uint8* Data, const int32 DataLength, int32* OutDataLength)
{
	if ((DataLength == 0) || (WritePipe == nullptr))
	{
		return false;
	}

	uint32 BytesWritten = 0;
	bool IsWritten = !!WriteFile(WritePipe, Data, DataLength, (::DWORD*) & BytesWritten, nullptr);

	if (OutDataLength)
	{
		*OutDataLength = (int32)BytesWritten;
	}

	return IsWritten;
}

bool WindowsProcess::DeleteInterprocessSynchObject(PlatformSemaphore* Object)
{
	if (nullptr == Object)
	{
		return false;
	}

	WindowsSemaphore* WinSem = static_cast<WindowsSemaphore*>(Object);
	CHECK(WinSem);

	::HANDLE Semaphore = WinSem->GetSemaphore();
	bool Success = false;
	if (Semaphore)
	{
		Success = (::CloseHandle(Semaphore) == TRUE);
		if (!Success)
		{
			DWORD Error = ::GetLastError();
			LOG(Warn, WindowsProcess, TEXTS("CloseHandle() for semaphore object %s failed, LastError = %d"), Object->GetName(), Error);
		}
	}

	delete WinSem;

	return Success;
}