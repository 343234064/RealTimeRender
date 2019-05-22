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


//Cache buffer size need to be power of 2
#ifndef FILE_WRITER_CACHEBUFFER_SIZE
#define FILE_WRITER_CACHEBUFFER_SIZE 4096
#endif
#ifndef FILE_READER_CACHEBUFFER_SIZE
#define FILE_READER_CACHEBUFFER_SIZE 4096
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
	static Serializer* CreateFileWriter(const TChar* FileName, uint32 Flag, uint32 CachedBufferSize = FILE_WRITER_CACHEBUFFER_SIZE);
	static Serializer* CreateFileReader(const TChar* FileName, uint32 Flag, uint32 CachedBufferSize = FILE_READER_CACHEBUFFER_SIZE);

};





class FileReadSerializer : public Serializer
{
public:
	FileReadSerializer(FileHandle* Filehandle, uint32 CacheBufferSize = FILE_READER_CACHEBUFFER_SIZE) :
		HandlePtr(Filehandle),
		CacheDataPos(0),
		CacheSize(0),
		BufferSize(CacheBufferSize),
		BufferPtr(nullptr)
	{
		BufferPtr = new uint8[BufferSize];
		CHECK(BufferPtr != nullptr);
	}

	~FileReadSerializer()
	{
		Close();
		if (BufferPtr != nullptr)
		{
			delete BufferPtr;
		}
	}


	void Serialize(void* Data, int64 Length) override;

	void Close() override { HandlePtr.reset(); }
	int64 Size() override { return HandlePtr->Size(); }
	int64 Pos() override { return HandlePtr->Pos(); }
	
	void  Seek(int64 Pos) override 
	{ 
		HandlePtr->Seek(Pos);

		CacheDataPos = Pos;
		CacheSize = 0;
	}


	String GetSerializerName() const override
	{
		return HandlePtr->GetFileName();
	}


protected:
	bool PreCache(int64& CurrentFilePos, int64& CurrentFileSize, int64& PreCacheStartPos);


protected:
	UniPTRFileHandle HandlePtr;
	
	//Pre-cached buffer
	int64  CacheDataPos; //The pos of cached data in file 
	int64  CacheSize; //Cached data size
	uint32  BufferSize; //Total buffer size
	uint8*  BufferPtr;

};



class FileWriteSerializer : public Serializer
{
public:
	FileWriteSerializer(FileHandle* Filehandle, uint32 CacheBufferSize = FILE_WRITER_CACHEBUFFER_SIZE):
		HandlePtr(Filehandle),
		CacheDataPos(0),
		CacheSize(0),
		BufferSize(CacheBufferSize),
		BufferPtr(nullptr),
		GetLogError(false)
	{
		BufferPtr = new uint8[BufferSize];
		CHECK(BufferPtr != nullptr);
	}

	~FileWriteSerializer()
	{
		Close();
		if (BufferPtr != nullptr)
		{
			delete BufferPtr;
		}
	}


	void Serialize(void* Data, int64 Length) override;

	void Close() override { Flush(); HandlePtr.reset(); }
	int64 Size() override { Flush(); return HandlePtr->Size(); }
	int64 Pos() override { return HandlePtr->Pos(); }


	void Flush() override;

	void  Seek(int64 Pos) override
	{
		Flush();
		HandlePtr->Seek(Pos);
	}


	String GetSerializerName() const override
	{
		return HandlePtr->GetFileName();
	} 

protected:
	//LOG macro will call FileWriteSerializer->Serialize() to log to file
	//Guarded Log prevent logging causes another log error which will lead to a stack overflow
	void GuardedLog(const TChar* Message)
	{
		if (!GetLogError)
		{
			GetLogError = true;
			//log
			GetLogError = false;
		}

	}


protected:
	UniPTRFileHandle HandlePtr;

	//Pre-cached buffer
	int64  CacheDataPos; //The pos of cached data in file 
	int64  CacheSize; //Cached data size
	uint32  BufferSize; //Total buffer size
	uint8*  BufferPtr;
	bool GetLogError;
};

