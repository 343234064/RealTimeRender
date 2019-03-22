/**********************
  Lock Guard

***********************/
#pragma once


template <typename GuardObject>
class LockGuard
{
public:
	explicit 
	LockGuard(GuardObject& InObjRef):
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