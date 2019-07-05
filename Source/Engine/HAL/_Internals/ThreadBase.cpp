#include "Global/Utilities/Assertion.h"
#include "HAL/Thread/ThreadBase.h"
#include "HAL/Thread/Thread.h"




Thread* Thread::Create(Runnable* ObjectToRun,
	                   const ANSICHAR* InitThreadName,
	                   uint32 InitStackSize,
	                   ThreadPriority InitPriority,
	                   uint64 AffinityMask)
{                  
	CHECK(ObjectToRun != nullptr);

	Thread* NewThread = nullptr;
	NewThread = PlatformThread::CreateThread();

	if (NewThread)
	{
		if (!NewThread->PlatformInit(ObjectToRun, InitThreadName, InitStackSize, InitPriority, AffinityMask))
		{
			delete NewThread;
			NewThread = nullptr;

			//log
		}
	}
	
	return NewThread;
}

