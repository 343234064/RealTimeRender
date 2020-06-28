/************************************
File manager


*************************************/

#pragma once

#include "Global/Utilities/Serializer.h"
#include "Global/Utilities/String.h"
#include "Global/Utilities/Assertion.h"
#include <memory>


#if PLATFORM_WINDOWS
#include "HAL/Platforms/Windows/WindowsFile.h"
#elif PLATFORM_LINUX
#include "HAL/Platforms/Linux/LinuxFile.h"
#elif PLATFORM_MAC
#include "HAL/Platforms/Mac/MacFile.h"
#else
#error Unknown platform
#endif


enum FileFlag
{
	//Shared Read mode
	SHARED_READ = 0x01,

	//Shared Write mode
	SHARED_WRITE = 0x02,

	//Do not overwrite the file in write mode when the file exists
	APPEND = 0x04,

	//Async read mode
	READASYNC = 0x08

};



struct FileManage
{
	static Serializer* CreateFileWriter(const TChar* FileName, uint32 Flag, uint32 CachedBufferSize = FILE_WRITER_CACHEBUFFER_SIZE)
	{
		FileHandle* Handle = PlatformFile::Open(FileName, AccessType::FOR_WRITE, true, !!(Flag & FileFlag::SHARED_READ), !!(Flag & FileFlag::APPEND));

		if (Handle == nullptr)
		{
			//log
			return nullptr;
		}

		return new FileWriteSerializer(Handle, CachedBufferSize);
	}

	static Serializer* CreateFileReader(const TChar* FileName, uint32 Flag, uint32 CachedBufferSize = FILE_READER_CACHEBUFFER_SIZE)
	{
		FileHandle* Handle = nullptr;
		if (!!(Flag & FileFlag::READASYNC))
		{
			Handle = PlatformFile::OpenReadAsync(FileName, !!(Flag & FileFlag::SHARED_WRITE));
		}
		else
		{
			Handle = PlatformFile::Open(FileName, AccessType::FOR_READ, !!(Flag & FileFlag::SHARED_WRITE), true, false);

		}

		if (Handle == nullptr)
		{
			//log
			return nullptr;
		}

		return new FileReadSerializer(Handle, CachedBufferSize);
	}

};




