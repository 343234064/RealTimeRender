/************************************
File handle for platforms


*************************************/
#pragma once

#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Global/Utilities/String.h"

#include <memory>


enum class AccessType
{
	//This file handle mainly used for FileReader
	FOR_READ = 0,

	//This file handle mainly used for FileWriter
	FOR_WRITE = 1
};


class FileHandle
{
public:
	FileHandle() {}
	virtual ~FileHandle()
	{}

	virtual int64 Pos() = 0;
	virtual bool  Seek(int64 Position) = 0;
	virtual bool  SeekFromEnd(int64 PositionFromEnd = 0) = 0;
	virtual bool  Read(uint8* Dest, int64 BytesToRead) = 0;
	virtual bool  Write(const uint8* Src, int64 BytesToWrite) = 0;
	virtual void  Flush() {}
	virtual int64 Size() = 0;
	virtual String GetFileName() const = 0;


};

typedef std::unique_ptr<FileHandle> UniPTRFileHandle;



