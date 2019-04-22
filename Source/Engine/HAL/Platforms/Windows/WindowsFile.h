/************************************
File handle for Windows


*************************************/
#pragma once

#include "Global/Utilities/String.h"
#include "Global/Utilities/Assertion.h"
#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "HAL/File/FileHandle.h"
#include "HAL/File/Path.h"




class WindowsFileHandle : public FileHandle
{
public:
	WindowsFileHandle(HANDLE Handle = NULL):
		FileHandle(Handle),
		FilePos(0),
		FileSize(0)
	{
		if (IsInit())
		{
			LARGE_INTEGER Int;
			GetFileSizeEx(FileHandle, &Int);
			FileSize = Int.QuadPart;
		}
		Memory::Zero(&Overlapped, sizeof(OVERLAPPED));
	}
	~WindowsFileHandle()
	{
		if (IsInit())
		{
			CloseHandle(FileHandle);
			FileHandle = NULL;
		}
	}


	bool IsInit()
	{
		return FileHandle != NULL && FileHandle != INVALID_HANDLE_VALUE;
	}

	int64 Pos() override { CHECK(IsInit()); return FilePos; }
	int64 Size() override { CHECK(IsInit()); return FileSize; }

	bool  Seek(int64 Position) override 
	{ 
		CHECK(IsInit());  
		CHECK(Position >= 0 && Position <= FileSize);

		FilePos = Position;

		ULARGE_INTEGER Int;
		Int.QuadPart = FilePos;
		Overlapped.Offset = Int.LowPart;
		Overlapped.OffsetHigh = Int.HighPart;

		return true;
	}

	bool  SeekFromEnd(int64 PositionFromEnd = 0) override
	{
		return Seek(FileSize - PositionFromEnd);
	}


	bool  Read(uint8* Dest, int64 BytesToRead) override
	{
		CHECK(IsInit());
		uint32 ReadCount = 0;

		if (!::ReadFile(FileHandle, Dest, BytesToRead, (DWORD*)&ReadCount, &Overlapped))
		{
			if (::GetLastError() != ERROR_IO_PENDING)
				return false;
			
			ReadCount = 0;
			if (!GetOverlappedResult(FileHandle, &Overlapped, (DWORD*)&ReadCount, true))
			{
				return false;
			}
		}

		FilePos += ReadCount;
		
		ULARGE_INTEGER Int;
		Int.QuadPart = FilePos;
		Overlapped.Offset = Int.LowPart;
		Overlapped.OffsetHigh = Int.HighPart;

		return true;
	}


	bool  Write(const uint8* Src, int64 BytesToWrite) override
	{
		CHECK(IsInit());
		uint32 WriteCount = 0;

		if (!::WriteFile(FileHandle, Src, BytesToWrite, (DWORD*)&WriteCount, NULL))
		{
			if (::GetLastError() != ERROR_IO_PENDING)
				return false;

			WriteCount = 0;
			if (!GetOverlappedResult(FileHandle, &Overlapped, (DWORD*)&WriteCount, true))
			{
				return false;
			}
		}

		FilePos += WriteCount;
		
		ULARGE_INTEGER Int;
		Int.QuadPart = FilePos;
		Overlapped.Offset = Int.LowPart;
		Overlapped.OffsetHigh = Int.HighPart;

		FileSize = std::max(FilePos, FileSize);

		return true;
	}


	void Flush() override {}
	


protected:
	HANDLE FileHandle;
	OVERLAPPED Overlapped; //This only makes sense when FileHandle is created with FILE_FLAG_OVERLAPPED
	int64 FilePos;
	int64 FileSize;
};





struct WindowsFile
{
public:
	//Synchronous I/O 
	static FileHandle* Open(const TChar* FileName, bool AllowWrite = true, bool AllowRead = true, bool OverWriteIfExist = false)
	{
		uint32 Access = (AllowWrite ? GENERIC_WRITE : 0) | (AllowRead ? GENERIC_READ : 0);
		uint32 ShareMode = (AllowWrite ? FILE_SHARE_WRITE : 0) | (AllowRead ? FILE_SHARE_READ : 0);
		uint32 CreationFlags = 0;

		if (AllowRead && !AllowWrite)
			CreationFlags = OPEN_EXISTING; //Opens a file or device, only if it exists
		else 
			CreationFlags = OverWriteIfExist ? 
			                CREATE_ALWAYS : //Creates a new file, always.  If the specified file exists and is writable, the function overwrites the file
			                OPEN_ALWAYS; //Opens a file, always. If the specified file does not exist and is a valid path to a writable location, the function creates a file

	
		HANDLE Handle = CreateFileW(*Path::NormalizeFilePath(FileName), Access, ShareMode, NULL, CreationFlags, FILE_ATTRIBUTE_NORMAL, NULL);
		if (Handle != INVALID_HANDLE_VALUE)
		{
			return new WindowsFileHandle(Handle);
		}

		return nullptr;
	}

	
	static FileHandle* OpenReadAsync(const TChar* FileName, bool AllowWrite = false)
	{
		uint32 Access = GENERIC_READ;
		uint32 ShareMode = (AllowWrite ? FILE_SHARE_WRITE : 0) |  FILE_SHARE_READ ;
		uint32 CreationFlags = OPEN_EXISTING;


		HANDLE Handle = CreateFileW(*Path::NormalizeFilePath(FileName), Access, ShareMode, NULL, CreationFlags, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		if (Handle != INVALID_HANDLE_VALUE)
		{
			return new WindowsFileHandle(Handle);
		}

		return nullptr;
	}

};

typedef WindowsFile PlatformFile;
