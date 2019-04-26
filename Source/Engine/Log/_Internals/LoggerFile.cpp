#include "Log/LoggerFile.h"
#include "HAL/Time.h"
#include "Global/Utilities/Misc.h"

ThreadWriter::ThreadWriter(Serializer* FileSerializer):
	FileSerializerPtr(FileSerializer),
	WriterThreadPtr(nullptr),
	DataStartPos(0),
	DataEndPos(0),
	StopTrigger(0),
	WriteRequestCounter(0),
	FlushRequestCounter(0),
	LastFlushTime(0.0),
	FlushInterval(0.2)
{
	RingBuffer.AddUninitialize(INIT_RINGBUFFER_SIZE);

    ANSICHAR ThreadName[64];
	std::sprintf(ThreadName, "ThreadWriter_%s", *(FileSerializerPtr->GetSerializerName()));

	PlatformAtomics::InterlockedExchangePtr((void**)&WriterThreadPtr, PlatformThread::Create(this, ThreadName, 0, ThreadPriority::BelowNormal));
	CHECK(WriterThreadPtr != nullptr);
}


ThreadWriter::~ThreadWriter()
{
	Flush();
	if (WriterThreadPtr != nullptr)
	{
		delete WriterThreadPtr;
		WriterThreadPtr = nullptr;
	}
}


bool ThreadWriter::Init()
{
	return true;
}


uint32 ThreadWriter::Run()
{
	while (StopTrigger.GetCounter() == 0)
	{
		if (WriteRequestCounter.GetCounter() > 0)
		{
			InternalSerialize();
		}
		//Flush interval
		else if ((PlatformTime::Time_Seconds()-LastFlushTime) > FlushInterval)
		{
			InternalFlush();
		}
		else
		{
			Platform::Sleep(0.01f);
		}
	}

	return 0;
}


void ThreadWriter::Stop()
{
	StopTrigger.Increment();
}


void ThreadWriter::InternalSerialize()
{
	while (WriteRequestCounter.GetCounter() > 0)
	{
		const int32 ThisThreadDataStartPos = DataStartPos.GetCounter();
		const int32 ThisThreadDataEndPos = DataEndPos.GetCounter();

		if (ThisThreadDataStartPos <= ThisThreadDataEndPos)
		{
			//Copy straight forward
			FileSerializerPtr->Serialize(RingBuffer.Begin() + ThisThreadDataStartPos, ThisThreadDataEndPos - ThisThreadDataStartPos);
		}
		else
		{
			//Need to copy in ring
			FileSerializerPtr->Serialize(RingBuffer.Begin() + ThisThreadDataStartPos, RingBuffer.CurrentNum() - ThisThreadDataStartPos);
			FileSerializerPtr->Serialize(RingBuffer.Begin(), ThisThreadDataEndPos);
		}

		DataStartPos.SetCounter(ThisThreadDataEndPos);
		WriteRequestCounter.Decrement();

		//If the time large than a interval, flush it
		if((PlatformTime::Time_Seconds() - LastFlushTime) > FlushInterval)
		   InternalFlush();

		//If there has flush requests, flush it
		if (FlushRequestCounter.GetCounter() > 0)
		{
			InternalFlush();
			FlushRequestCounter.Decrement();
		}
	}
}


void ThreadWriter::InternalFlush()
{
	FileSerializerPtr->Flush();
	LastFlushTime = PlatformTime::Time_Seconds();
}






void ThreadWriter::Serialize(void* Data, int64 Length)
{
	CHECK(WriterThreadPtr != nullptr);
	if (Data == nullptr || Length <= 0) return;

	const uint8* Bytes = (uint8*)Data;
	LockGuard<PlatformCriticalSection> Lock(CriticalSection);

	//This 2 value may be modified by writer thread, so must be atomic
	const int32 ThisThreadDataStartPos = DataStartPos.GetCounter();
	const int32 ThisThreadDataEndPos = DataEndPos.GetCounter();

	const int32 RingBufferFreeSize = ThisThreadDataStartPos <= ThisThreadDataEndPos ?
		                             (RingBuffer.CurrentNum() - (ThisThreadDataEndPos - ThisThreadDataStartPos)) :
		                             (ThisThreadDataStartPos - ThisThreadDataEndPos);

	//If no enough space, flush and add new space if needed
	if (RingBufferFreeSize <= Length)
	{
		//Flush buffer
		WriteRequestCounter.Increment();
		if (!WriterThreadPtr) InternalSerialize();

		//Doesn't force the flush() call, just wait for writer thread to flush
		while (WriteRequestCounter.GetCounter() != 0)
		{
			//Switch to thread
			Platform::Sleep(0);
		}
		CHECK(WriteRequestCounter.GetCounter() == 0);

		if (Length >= RingBuffer.CurrentNum())
			//Add 1 to avoid mod% return 0
			RingBuffer.AddUninitialize((int32)Length + 1 - RingBuffer.CurrentNum());
	}

	//Has enough space
	const int32 WritePos = ThisThreadDataEndPos;
	if ((WritePos + Length) <= RingBuffer.CurrentNum())
	{
		//Copy straight forward
		Memory::Copy(RingBuffer.Begin() + WritePos, Data, Length);
	}
	else
	{
		//Need to copy in ring
		const int32 SizeToEnd = RingBuffer.CurrentNum() - WritePos;
		Memory::Copy(RingBuffer.Begin() + WritePos, Bytes, SizeToEnd);
		Memory::Copy(RingBuffer.Begin(), Bytes + SizeToEnd, Length - SizeToEnd);
	}

	DataEndPos.SetCounter((ThisThreadDataEndPos + Length) % RingBuffer.CurrentNum());
	WriteRequestCounter.Increment();

}


void ThreadWriter::Flush()
{
	LockGuard<PlatformCriticalSection> Lock(CriticalSection);
	
	//Call Flush() means there is a flush request and alos a write request
	FlushRequestCounter.Increment();
	WriteRequestCounter.Increment();
	
	//If WriterThread failed to create, serialize immediately to avoid a infinite loop
	if (!WriterThreadPtr) InternalSerialize();
	
	//Doesn't force the flush() call, just wait for writer thread to flush
	while (WriteRequestCounter.GetCounter() != 0)
	{
		//Switch to thread
		Platform::Sleep(0);
	}

	CHECK(WriteRequestCounter.GetCounter() == 0);
}








LoggerFile::LoggerFile(const TChar* OutputFileName, bool AppendIfExist):
	WriterPtr(nullptr),
	FilePtr(nullptr),
	AppendExist(AppendIfExist),
    FileOpened(false)
{
	if (OutputFileName != nullptr)
	{
		PlatformChars::Strncpy(FileName, OutputFileName, ARRAY_SIZE(FileName));
	}
	else
	{
		FileName[0] = 0;
	}
}


LoggerFile::~LoggerFile()
{
	Shutdown();
}



void LoggerFile::SetFileName(const TChar* OutputFileName)
{
	Shutdown();

	PlatformChars::Strncpy(FileName, OutputFileName, ARRAY_SIZE(FileName));
}


void LoggerFile::Shutdown()
{
	if (WriterPtr != nullptr)
	{
		delete WriterPtr;
		WriterPtr = nullptr;
	}

	if (FilePtr != nullptr)
	{
		delete FilePtr;
		FilePtr = nullptr;
	}

	FileName[0] = 0;
	FileOpened = false;
}


void LoggerFile::Serialize(LogVerbosity Verbosity, const TChar* Data)
{
	static bool ErrorEntry = false;
	if (!gIsGetCriticalError || ErrorEntry)
	{
		if (!WriterPtr)
		{
			if (!FileName[0])
			{
				TChar DefaultName[] = TEXTS("LogFileOutput.txt");
				PlatformChars::Strcpy(FileName, ARRAY_SIZE(FileName), DefaultName);
			}

			if (InitWriterThread())
			{
				FileOpened = true;

			}
			else
			{
				//log
			}
		}

		if (WriterPtr)
		{
			//Serialize
		}
		
	}
	else
	{
		ErrorEntry = true;
		Serialize(Verbosity, Data);
		ErrorEntry = false;
	}
}


bool LoggerFile::InitWriterThread()
{
	uint32 FileFlag = FileFlag::READ | (AppendExist ? FileFlag::APPEND : 0);
	Serializer* FileSerializer = nullptr;

	FileManage::CreateFileWriter(FileName, FileFlag);

	ThreadWriter* Writer = nullptr;
	if (FileSerializer != nullptr)
	{
		FilePtr = FileSerializer;
		Writer = new ThreadWriter(FilePtr);
	}

	return !!Writer;
}