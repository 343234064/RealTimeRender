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



#ifndef FILE_WRITER_CACHEBUFFER_SIZE
#define FILE_WRITER_CACHEBUFFER_SIZE 4096
#endif



class FileWriter : public Serializer
{
public:
	FileWriter(const TChar* FileName, int64 Position, uint32 CacheBufferSize = FILE_WRITER_CACHEBUFFER_SIZE):
		FileName(FileName),
		Position(Position),
		BufferCount(0),
		BufferSize(CacheBufferSize),
		BufferPtr(nullptr)
	{
		BufferPtr = new uint8[BufferSize];
		CHECK(BufferPtr != nullptr);
	}

	~FileWriter()
	{
		Close();
		if (BufferPtr != nullptr)
		{
			delete BufferPtr;
		}
	}


	void Serialize(void* Data, uint64 Length) override;

	void Close();
	void Flush() override;
	int64 Size() override;
	int64 Pos() override { return Position; }
	void  Seek(int64 Pos) override;


	String GetArchiveName() const override
	{
		return FileName;
	} 


protected:
	String FileName;
	int64  Position;

	uint64  BufferCount;
	uint32  BufferSize;
	uint8*  BufferPtr;
	
};

