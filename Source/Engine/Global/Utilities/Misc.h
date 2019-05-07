/************************************
Misc Helpers


*************************************/
#pragma once

#include "HAL/Chars.h"


/************************************
Variable Arguments
*************************************/
#define GET_FORMAT_VARARGS(BufferPtr, BufferCount, MaxCountToWrite, ArgBehind, Format, Result) \
{ \
   va_list ArgPtr; \
   va_start(ArgPtr, ArgBehind); \
   Result = PlatformChars::VarArgSprintf(BufferPtr, BufferCount, MaxCountToWrite, Format, ArgPtr); \
   va_end(ArgPtr); \
   if(Result >= BufferCount) Result = -1; \
} 

#define CALL_FUNC_VARARGS(Func, ArgBehind) \
{ \
   va_list Args; \
   va_start(Args, ArgBehind); \
   Func; \
   va_end(Args); \
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