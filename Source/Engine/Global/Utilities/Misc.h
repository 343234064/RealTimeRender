/************************************
Misc Helpers


*************************************/
#pragma once

#include "HAL/Chars.h"


/************************************
Variable Arguments Macros
*************************************/
#define GET_FORMAT_VARARGS(BufferPtr, BufferCount, MaxCountToWrite, ArgBehind, Format, Result) \
{ \
   va_list ArgPtr; \
   va_start(ArgPtr, ArgBehind); \
   Result = PlatformChars::VarArgSprintf(BufferPtr, BufferCount, MaxCountToWrite, Format, ArgPtr); \
   va_end(ArgPtr); \
   if(Result >= BufferCount) Result = -1; \
} 

#define GET_FORMAT_VARARGS_NORESULT(BufferPtr, BufferCount, MaxCountToWrite, ArgBehind, Format) \
{ \
   va_list ArgPtr; \
   va_start(ArgPtr, ArgBehind); \
   PlatformChars::VarArgSprintf(BufferPtr, BufferCount, MaxCountToWrite, Format, ArgPtr); \
   va_end(ArgPtr); \
} 

#define CALL_FUNC_VARARGS(Func, ArgBehind) \
{ \
   va_list Args; \
   va_start(Args, ArgBehind); \
   Func; \
   va_end(Args); \
}

#define SRINTF_VARARGS(CallFunc) \
{ \
	const int32 BufferInitSize = 512; \
	TChar  PrintBuffer[BufferInitSize]; \
	TChar* BufferPtr = PrintBuffer; \
	int32  BufferSize = BufferInitSize; \
	int32  WroteCount = -1; \
	GET_FORMAT_VARARGS(BufferPtr, BufferSize, BufferSize - 1, Format, Format, WroteCount); \
	if (WroteCount == -1) \
	{ \
		BufferPtr = nullptr; \
		while (WroteCount == -1) \
		{ \
			BufferSize *= 2; \
			BufferPtr = (TChar*)Memory::Realloc(BufferPtr, BufferSize * sizeof(TChar)); \
			GET_FORMAT_VARARGS(BufferPtr, BufferSize, BufferSize - 1, Format, Format, WroteCount); \
		} \
	} \
\
	BufferPtr[WroteCount] = 0; \
\
	CallFunc; \
\
	if (BufferSize != BufferInitSize) \
		Memory::Free(BufferPtr); \
}


//==============================
//Helpers
//==============================
template <typename Type>
static inline
Type Clamp(const Type Value, const Type Min, const Type Max)
{
	return Value < Min ? Min : Value < Max ? Value : Max;
}






/**********************
  Lock Guard
***********************/
template <typename GuardObject>
class LockGuard
{
public:
	explicit
		LockGuard(GuardObject& InObjRef) :
		ObjRef(InObjRef)
	{
		ObjRef.Lock();
	}

	~LockGuard()
	{
		ObjRef.UnLock();
	}

	LockGuard(const LockGuard& Other) = delete;
	LockGuard& operator=(const LockGuard&) = delete;

private:
	LockGuard() {}

private:
	GuardObject& ObjRef;
};