#pragma once

#include "Global/GlobalType.h"


#define MAX_ERROR_MESSAGE_CHARS 2048
#define MAX_THREADS 256
#define MAX_THREAD_NAME_CHARS 64


enum class CrashType
{
	Crash,
	Assert,
	Ensure,
	GPUCrash,
	Hang,
	OutofMemory,
	AbnormalShutdown
};


struct CrashInfo
{
	const TChar* Message;

	CrashInfo(const TChar* InMessage) :
		Message(InMessage)
	{}
};


struct SharedCrashContext
{
	TChar					ErrorMessage[MAX_ERROR_MESSAGE_CHARS];
	uint32					ThreadIds[MAX_THREADS];
	TChar					ThreadNames[MAX_THREAD_NAME_CHARS * MAX_THREADS];
	uint32					NumThreads;
	uint32					CrashingThreadId;
	CrashType			CrashType;
};
