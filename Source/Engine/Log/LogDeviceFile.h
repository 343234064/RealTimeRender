/************************************
Log device which output to file


*************************************/
#pragma once

#include "Global/Utilities/DynamicArray.h"
#include "Global/Utilities/AtomicCounter.h"
#include "HAL/FileManage.h"
#include "HAL/Thread.h"
#include "Log/LogDevice.h"




#define INIT_RINGBUFFER_SIZE 128 * 1024

//Asynchronous Writer
class ThreadWriter:public Runnable
{
public:
	ThreadWriter(Serializer* FileSerializer);
	~ThreadWriter();

	/****Call in Thread****/
	bool Init() override;
	uint32 Run() override;
	void Stop() override;

	/****Call in Client****/
	//Send data to ring buffer which ready for write to file
	void Serialize(void* Data, int64 Length);
	void Flush();


private:
	/****Call in Thread****/
	//Serialize the buffer data to file
	void InternalSerialize();
	void InternalFlush();


private:
	Serializer* FileSerializerPtr;
	PlatformThread* WriterThreadPtr;

	Array<uint8> RingBuffer;
	AtomicCounter<int32> DataStartPos;
	AtomicCounter<int32> DataEndPos;
	PlatformCriticalSection CriticalSection;
	
	AtomicCounter<int32> StopTrigger;

	AtomicCounter<int32> WriteRequestCounter;
	AtomicCounter<int32> FlushRequestCounter;

	
	//Flush the data at a certain interval (0.2sec)
	double LastFlushTime;
	double FlushInterval;
};




class LogDeviceFile : public LogDevice
{
public:
	LogDeviceFile(const TChar* OutputFileName = nullptr, bool AppendIfExist = true);
	virtual ~LogDeviceFile()
	{
		Shutdown();
	}


	void Serialize(const TChar* Data) override;
	
	void Flush() override 
	{
		if(WriterPtr)
		   WriterPtr->Flush(); 
	}

	void Shutdown() override;

	void SetFileName(const TChar* OutputFileName); //Set file name will shutdown the file writer 
	const TChar* GetFileName() const { return FileName; }

	
protected:
	bool InitWriterThread();


protected:
	ThreadWriter* WriterPtr;
	Serializer* FilePtr;

	TChar FileName[512];
	bool AppendExist;
	bool InitFailed;

};



