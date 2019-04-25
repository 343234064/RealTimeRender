/************************************
Logger which output to file


*************************************/
#pragma once

#include "Global/Utilities/DynamicArray.h"
#include "Global/Utilities/AtomicCounter.h"
#include "HAL/File/FileManage.h"
#include "HAL/Thread/Thread.h"
#include "Log/Logger.h"






//Asynchronous Writer
class ThreadWriter:public Runnable
{
public:
	ThreadWriter();
	~ThreadWriter();

	bool Init() override;
	uint32 Run() override;
	void Stop() override;

	void SendToBuffer(void* Data, int64 Length);
	void Flush();


private:
	//Serialize the buffer data to file
	void Serialize();

private:
	Serializer* FileSerializerPtr;
	
	Array<uint8> RingBuffer;
	AtomicCounter<int32> DataStartPos;
	AtomicCounter<int32> DataEndPos;
	PlatformCriticalSection CriticalSection;
	
	AtomicCounter<int32> StopCounter;


};




class LoggerFile : public Logger
{
public:
	LoggerFile(const TChar* OutputFileName = nullptr, bool AppendIfExist = false);
	virtual ~LoggerFile();


	virtual void Serialize(LogVerbosity Verbosity, const TChar* Data) override;
	virtual void Flush() override;
	virtual void Shutdown() override;


	void SetFileName(const TChar* OutputFileName); //Set file name will shutdown the file writer 
	const TChar* GetFileName() const { return FileName; }

protected:
	ThreadWriter* WriterPtr;

	TChar FileName[1024];
	bool AppendIfExist;
	bool FileOpened;

};

