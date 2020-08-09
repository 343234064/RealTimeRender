#include "Global/Utilities/Assertion.h"
#include "HAL/Platforms/GenericThread.h"
#include "HAL/Thread.h"




Thread* Thread::Create(Runnable* ObjectToRun,
	                   const TChar* InitThreadName,
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

