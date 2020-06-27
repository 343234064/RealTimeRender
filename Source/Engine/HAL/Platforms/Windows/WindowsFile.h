/************************************
File handle for Windows


*************************************/
#pragma once

#include "Global/Utilities/String.h"
#include "Global/Utilities/Assertion.h"
#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "HAL/Platforms/GenericFile.h"
#include "HAL/Path.h"



//Do not add LOG macro here
class WindowsFileHandle : public FileHandle
{
public:
	WindowsFileHandle(const TChar* Filename, HANDLE Handle = NULL):
        FileName(Filename),
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
		CHECKF(Position >= 0 && Position <= FileSize, "Attempted to seek to a invalid location (%lld/%lld), file: %s.", Position, FileSize, *FileName);

		//FilePos = Clamp(Position, 0ll, FileSize);
		FilePos = Position;

		ULARGE_INTEGER Int;
		Int.QuadPart = FilePos;
		Overlapped.Offset = Int.LowPart;
		Overlapped.OffsetHigh = Int.HighPart;

		SetFilePointer(FileHandle, Int.LowPart, (PLONG)&Int.HighPart, FILE_BEGIN);

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

		if (!::ReadFile(FileHandle, Dest, (DWORD)BytesToRead, (DWORD*)&ReadCount, &Overlapped))
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
	    
		if (!::WriteFile(FileHandle, Src, (DWORD)BytesToWrite, (DWORD*)&WriteCount, NULL))
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
	
	FORCE_INLINE
	String GetFileName() const override { return FileName; }

protected:
	String FileName;
	HANDLE FileHandle;
	OVERLAPPED Overlapped; //This only makes sense when FileHandle is created with FILE_FLAG_OVERLAPPED
	int64 FilePos;
	int64 FileSize;
};





struct WindowsFile
{
public:
	//Synchronous I/O 
	static FileHandle* Open(const TChar* FileName, AccessType Type, bool ShareWrite, bool ShareRead, bool AppendIfExist = true)
	{
		uint32 Access = (Type == AccessType::FOR_WRITE) ? GENERIC_WRITE : GENERIC_READ;
		Access |= FILE_APPEND_DATA;
		uint32 ShareMode = 0;
		if(Type == AccessType::FOR_WRITE)
			ShareMode = FILE_SHARE_WRITE | (ShareRead ? FILE_SHARE_READ : 0);
		else 
			ShareMode = FILE_SHARE_READ | (ShareWrite ? FILE_SHARE_WRITE : 0);

		uint32 CreationFlags = 0;
		if (Type == AccessType::FOR_WRITE)
		{
			CreationFlags = AppendIfExist ?
				OPEN_ALWAYS :  //Opens a file, always. If the specified file does not exist and is a valid path to a writable location, the function creates a file
			    CREATE_ALWAYS; //Creates a new file, always.  If the specified file exists and is writable, the function overwrites the file
				
		}
		else
		{
			CreationFlags = OPEN_EXISTING; //Opens a file or device, only if it exists
		}
		
	
		HANDLE Handle = CreateFileW(*Path::NormalizeFilePath(FileName), Access, ShareMode, NULL, CreationFlags, FILE_ATTRIBUTE_NORMAL, NULL);
		if (Handle != INVALID_HANDLE_VALUE)
		{
			SetFilePointer(Handle, 0, 0, AppendIfExist ? FILE_END : FILE_BEGIN);
			return new WindowsFileHandle(FileName, Handle);
		}

		return nullptr;
	}

	
	static FileHandle* OpenReadAsync(const TChar* FileName, bool ShareWrite = false)
	{
		uint32 Access = GENERIC_READ;
		uint32 ShareMode = (ShareWrite ? FILE_SHARE_WRITE : 0) |  FILE_SHARE_READ ;
		uint32 CreationFlags = OPEN_EXISTING;


		HANDLE Handle = CreateFileW(*Path::NormalizeFilePath(FileName), Access, ShareMode, NULL, CreationFlags, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		if (Handle != INVALID_HANDLE_VALUE)
		{
			return new WindowsFileHandle(FileName, Handle);
		}

		return nullptr;
	}

};

typedef WindowsFile PlatformFile;
