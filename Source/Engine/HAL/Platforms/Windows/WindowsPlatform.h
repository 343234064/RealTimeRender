#pragma once

#include "Global/GlobalConfigs.h"

#if !PLATFORM_WINDOWS
#error Current Platform is NOT Windows or NO Platform macro
#endif

//Request Visual C++ 2017 or later
static_assert(_MSC_VER >= 1910, "Visual Studio 2017 or later is required to compile on Windows");

//Compiler Warning -> Error, to make the compile environment more stringent
//Some may not work due to the compiler version
//Check:https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/
//
// 4000 ~ 4199 SKIPPED:
// 4000 UNKNOWN WARNING
// 4061 enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label:						
// 4062 enumerator 'identifier' in switch of enum 'enumeration' is not handled
// 4100 'identifier': unreferenced formal parameter
// 4165 'HRESULT' is being converted to 'bool'; are you sure this is what you want?
#pragma warning(error:     4001 4002 4003 4004 4005 4006 4007 4008 4009 4010 4011 4012 4013 4014 4015 4016 4017 4018 4019 4020 4021 4022 4023 4024 4025 4026 4027 4028 4029 4030 4031 4032 4033 4034 4035 4036 4037 4038 4039 4040 4041 4042 4043 4044 4045 4046 4047 4048 4049 4050 4051 4052 4053 4054 4055 4056 4057 4058 4059 4060           4063 4064 4065 4066 4067 4068 4069 4070 4071 4072 4073 4074 4075 4076 4077 4078 4079 4080 4081 4082 4083 4084 4085 4086 4087 4088 4089 4090 4091 4092 4093 4094 4095 4096 4097 4098 4099)
#pragma warning (error:      4101 4102 4103 4104 4105 4106 4107 4108 4109 4110 4111 4112 4113 4114 4115 4116 4117 4118 4119 4120 4121 4122 4123 4124 4125 4126 4127 4128 4129 4130 4131 4132 4133 4134 4135 4136 4137 4138 4139 4140 4141 4142 4143 4144 4145 4146 4147 4148 4149 4150 4151 4152 4153 4154 4155 4156 4157 4158 4159 4160 4161 4162 4163 4164      4166 4167 4168 4169 4170 4171 4172 4173 4174 4175 4176 4177 4178 4179 4180 4181 4182 4183 4184 4185 4186 4187 4188 4189 4190 4191 4192 4193 4194 4195 4196 4197 4198 4199)
// 4200 ~ 4399 SKIPPED:
// 4242 'identifier' : conversion from 'type1' to 'type2', possible loss of data												
// 4254 'operator' : conversion from 'type1' to 'type2', possible loss of data																							
// 4266 'virtual_function': no override available for virtual member function from base 'classname'; function is hidden
// 4305 'identifier' : truncation from 'type1' to 'type2'																																				
// 4311 'variable' : pointer truncation from 'type' to 'type'																	
// 4312 'operation' : conversion from 'type1' to 'type2' of greater size														
// 4339 'type' : use of undefined type detected in CLR meta-data - use of this type may lead to a runtime exception												
// 4345 behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized																	
// 4365 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch												
// 4370 layout of class has changed from a previous version of the compiler due to better packing
// 4371 'classname': layout of class may have changed from a previous version of the compiler due to better packing of member 'member'
// 4373 'function': virtual function overrides 'base_function', previous versions of the compiler did not override when parameters only differed by const/volatile qualifiers
// 4388 'expression': signed/unsigned mismatch
#pragma warning (error: 4200 4201 4202 4203 4204 4205 4206 4207 4208 4209 4210 4211 4212 4213 4214 4215 4216 4217 4218 4219 4220 4221 4222 4223 4224 4225 4226 4227 4228 4229 4230 4231 4232 4233 4234 4235 4236 4237 4238 4239 4240 4241      4243 4244 4245 4246 4247 4248 4249 4250 4251 4252 4253      4255 4256 4257 4258 4259 4260 4261 4262 4263 4264 4265      4267 4268 4269 4270 4271 4272 4273 4274 4275 4276 4277 4278 4279 4280 4281 4282 4283 4284 4285 4286 4287 4288 4289 4290 4291 4292 4293 4294 4295 4296 4297 4298 4299)
#pragma warning (error: 4300 4301 4302 4303 4304      4306 4307 4308 4309 4310           4313 4314 4315 4316 4317 4318 4319 4320 4321 4322 4323 4324 4325 4326 4327 4328 4329 4330 4331 4332 4333 4334 4335 4336 4337 4338      4340 4341 4342 4343 4344      4346 4347 4348 4349 4350      4352 4353 4354 4355 4356 4357 4358 4359 4360 4361 4362 4363 4364      4366 4367 4368 4369           4372      4374 4375 4376 4377 4378 4379 4380 4381 4382 4383 4384 4385 4386 4387      4389 4390 4391 4392 4393 4394 4395 4396 4397 4398 4399)
// 4400 ~ 4599 SKIPPED:			
// 4412 'function': function signature contains type 'type'; C++ objects are unsafe to pass between pure code and mixed or native
// 4435 'class1' : Object layout under /vd2 will change due to virtual base 'class2'										
// 4444 'identifier': top level '__unaligned' is not implemented in this context
// 4471 'enumeration': a forward declaration of an unscoped enumeration must have an underlying type (int assumed)
// 4472 'identifier' is a native enum: add an access specifier (private/public) to declare a 'WinRT|managed' enum
// 4481 nonstandard extension used: override specifier 'keyword'					
// 4571 Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
// 4574 'Identifier' is defined to be '0': did you mean to use '#if identifier'?
// 4592 'function': 'constexpr' call evaluation failed; function will be called at run-time
// 4599 'flag path': command line argument number number does not match precompiled header
#pragma warning (error: 4400 4401 4402 4403 4404 4405 4406 4407 4408 4409 4410 4411      4413 4414 4415 4416 4417 4418 4419 4420 4421 4422 4423 4424 4425 4426 4427 4428 4429 4430 4431 4432 4433 4434      4436 4437 4438 4439 4440 4441 4442 4443      4445 4446 4447 4448 4449 4450 4451 4452 4453 4454 4455 4456 4457 4458 4459 4460 4461 4462 4463 4464 4465 4466 4467 4468 4469 4470           4473 4474 4475 4476 4477 4478 4479 4480      4482 4483 4484 4485 4486 4487 4488 4489 4490 4491 4492 4493 4494 4495 4496 4497 4498 4499)
#pragma warning (error: 4500 4501 4502 4503 4504 4505 4506 4507 4508 4509 4510 4511 4512 4513 4514 4515 4516 4517 4518 4519 4520 4521 4522 4523 4524 4525 4526 4527 4528 4529 4530 4531 4532 4533 4534 4535 4536 4537 4538 4539 4540 4541 4542 4543 4544 4545 4546 4547 4548 4549 4550 4551 4552 4553 4554 4555 4556 4557 4558 4559 4560 4561 4562 4563 4564 4565 4566 4567 4568 4569 4570      4572 4573      4575 4576 4577 4578 4579 4580 4581 4582 4583 4584 4585 4586 4587 4588 4589 4590 4591      4593 4594 4595 4596 4597 4598     )
// 4600 ~ 4799 SKIPPED:		
// 4605 '/Dmacro' specified on current command line, but was not specified when precompiled header was built
// 4608 'union_member' has already been initialized by another union member in the initializer list, 'union_member'
// 4619 #pragma warning: there is no warning number 'number'
// 4625 'derived class': copy constructor was implicitly defined as deleted because a base class copy constructor is inaccessible or deleted
// 4626 'derived class': assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted
// 4651 'definition' specified for precompiled header but not for current compile
// 4668 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directive'
// 4692 'function': signature of non-private member contains assembly private native type 'native_type'
// 4710 function': function not inlined
// 4730 'main': mixing _m64 and floating point expressions may result in incorrect code
// 4738 storing 32-bit float result in memory, possible loss of performance
// 4767 section name '%s' is longer than 8 characters and will be truncated by the linker
// 4768 __declspec attributes before linkage specification are ignored
// 4770 partially validated enum 'name' used as index
// 4772 'string' : format string expected in argument number is not a string literal
#pragma warning (error: 4600 4601 4602 4603 4604      4606 4607      4609 4610 4611 4612 4613 4614 4615 4616 4617 4618      4620 4621 4622 4623 4624           4627 4628 4629 4630 4631 4632 4633 4634 4635 4636 4637 4638 4639 4640 4641 4642 4643 4644 4645 4646 4647 4648 4649 4650      4652 4653 4654 4655 4656 4657 4658 4659 4660 4661 4662 4663 4664 4665 4666 4667      4669 4670 4671 4672 4673 4674 4675 4676 4677 4678 4679 4680 4681 4682 4683 4684 4685 4686 4687 4688 4689 4690 4691      4693 4694 4695 4696 4697 4698 4699)
#pragma warning (error: 4700 4701 4702 4703 4704 4705 4706 4707 4708 4709           4712 4713 4714 4715 4716 4717 4718 4719 4720 4721 4722 4723 4724 4725 4726 4727 4728 4729      4731 4732 4733 4734 4735 4736 4737      4739 4740 4741 4742 4743 4744 4745 4746 4747 4748 4749 4750 4751 4752 4753 4754 4755 4756 4757 4758 4759 4760 4761 4762 4763 4764 4765 4766           4769      4771 4772 4773      4775 4776 4777 4778 4779 4780 4781 4782 4783 4784 4785 4786 4787 4788 4789 4790 4791 4792 4793 4794 4795 4796 4797 4798 4799)
// 4800 ~ 4999 SKIPPED:	
// 4820 'bytes' bytes padding added after construct 'member_name'
// 4837 trigraph detected: '??character' replaced by 'character'
// 4838 conversion from 'type_1' to 'type_2' requires a narrowing conversion
// 4866 compiler may not enforce left-to-right evaluation order for call to operator_name
// 4868 'file(line_number)' compiler may not enforce left-to-right evaluation order in braced initialization list
// 4962 'function': Profile-guided optimizations disabled because optimizations caused profile data to become inconsistent
// 4987 nonstandard extension used: 'throw (...)'
// 4988 'variable': variable declared outside class/function scope
// 4996 'description': message
#pragma warning (error: 4800 4801 4802 4803 4804 4805 4806 4807 4808 4809 4810 4811 4812 4813 4814 4815 4816 4817 4818 4819      4821 4822 4823 4824 4825 4826 4827 4828 4829 4830 4831 4832 4833 4834 4835 4836           4839 4840 4841 4842 4843 4844 4845 4846 4847 4848 4849 4850 4851 4852 4853 4854 4855 4856 4857 4858 4859 4860 4861 4862 4863 4864 4865      4867      4869 4870 4871 4872 4873 4874 4875 4876 4877 4878 4879 4880 4881 4882 4883 4884 4885 4886 4887 4888 4889 4890 4891 4892 4893 4894 4895 4896 4897 4898 4899)
#pragma warning (error: 4900 4901 4902 4903 4904 4905 4906 4907 4908 4909 4910 4911 4912 4913 4914 4915 4916 4917 4918 4919 4920 4921 4922 4923 4924 4925 4926 4927 4928 4929 4930 4931 4932 4933 4934 4935 4936 4937 4938 4939 4940 4941 4942 4943 4944 4945 4946 4947 4948 4949 4950 4951 4952 4953 4954 4955 4956 4957 4958 4959 4960 4961      4963 4964 4965 4966 4967 4968 4969 4970 4971 4972 4973 4974 4975 4976 4977 4978 4979 4980 4981 4982 4983 4984 4985 4986           4989 4990 4991 4992 4993 4994 4995      4997 4998 4999)
// 4999 ~ .... SKIPPED:	
// 5024 'type': move constructor was implicitly defined as deleted
// 5025 'type': move assignment operator was implicitly defined as deleted
// 5026 'type': move constructor was implicitly defined as deleted
// 5027 'type': move assignment operator was implicitly defined as deleted
// 5031 #pragma warning(pop): likely mismatch, popping warning state pushed in different file
// 5032 detected #pragma warning(push) with no corresponding #pragma warning(pop)
// 5034 use of intrinsic 'intrinsic' causes function function to be compiled as guest code
// 5035 use of feature 'feature' causes function function to be compiled as guest code
// 5038 data member 'member1' will be initialized after data member 'member2'
// 5039 'function': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
// 5040 dynamic exception specifications are valid only in C++14 and earlier; treating as noexcept(false)
// 5041 'definition': out-of-line definition for constexpr static data member is not needed and is deprecated in C++17
// 5045 Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
// 5048 Use of macro 'macroname' may result in non-deterministic output
// 5100 __VA_ARGS__ is reserved for use in variadic macros
// 5101 use of preprocessor directive in function-like macro argument list is undefined behavior
// 5105 macro expansion producing 'defined' has undefined behavior
#pragma warning (error: /*5000~5021 Unknown*/ 5022 5023                     5028 5029 5030           5033           5036 5037                     5042 5043 5044      5046 5047      5049 5050 /*5051~5099 Unknown*/           5102 5103 5104      5106 5107)

//Set this warning to ON
#pragma warning (1:4100 4165 4625 4626 4710)

//C4711 The compiler performed inlining on the given function
//This warning is informational
#pragma warning (1:4711)



// WIN32_LEAN_AND_MEAN excludes rarely-used services from windows headers.
#define WIN32_LEAN_AND_MEAN
// The below excludes some other unused services from the windows headers
#define NOGDICAPMASKS			// CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define OEMRESOURCE				// OEM Resource values
#define NOATOM					// Atom Manager routines
#define NODRAWTEXT				// DrawText() and DT_*
#define NOKERNEL				// All KERNEL #defines and routines
#define NOMEMMGR				// GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE				// typedef METAFILEPICT
#define NOMINMAX				// Macros min(a,b) and max(a,b)
#define NOOPENFILE				// OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL				// SB_* and scrolling routines
#define NOSERVICE				// All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND					// Sound driver routines
#define NOCOMM					// COMM driver routines
#define NOKANJI					// Kanji support stuff.
#define NOHELP					// Help engine interface.
#define NOPROFILER				// Profiler interface.
#define NODEFERWINDOWPOS		// DeferWindowPos routines
#define NOMCX					// Modem Configuration Extensions
#define NOCRYPT
#define NOTAPE
#define NOIMAGE
#define NOPROXYSTUB
#define NORPC
#define UNICODE                 //Use unicode
#include <Windows.h>

//Undo Windows definations which may be repeated
#if defined(int8)
#undef int8
#endif
#if defined(uint8)
#undef uint8
#endif
#if defined(int16)
#undef int16
#endif
#if defined(uint16)
#undef uint16
#endif
#if defined(uint32)
#undef uint32
#endif
#if defined(int32)
#undef int32
#endif
#if defined(uint64)
#undef uint64
#endif
#if defined(int64)
#undef int64
#endif

#if defined(InterlockedIncrement)
#undef InterlockedIncrement
#endif
#if defined(InterlockedDecrement)
#undef InterlockedDecrement
#endif
#if defined(InterlockedAdd)
#undef InterlockedAdd
#endif
#if defined(InterlockedExchange)
#undef InterlockedExchange
#endif
#if defined(InterlockedCompareExchange)
#undef InterlockedCompareExchange
#endif



//Allocator
#define PLATFORM_ALLOC_SUPPORT_TBB 1
#define PLATFORM_ALLOC_SUPPORT_JEM 1
#define PLATFORM_ALLOC_USE_TBB 1
#define PLATFORM_ALLOC_USE_JEM 0

//Using wchar_t
//wchar_t will be utf-16/2Bytes in windows
#define PLATFORM_TEXT_IS_WCHAR 1


#define FORCE_INLINE __forceinline
#define FORCE_NO_INLINE 


#include <intrin.h>

//Current instance handle
extern "C" HINSTANCE gMainInstanceHandle;


class WindowsTypes
{
public:
	typedef	signed char		int8;
	typedef unsigned char		uint8;

	typedef signed short int		int16;
	typedef unsigned short int	uint16;

	typedef signed int	 		int32;
	typedef unsigned int			uint32;

	typedef signed long long		int64;
	typedef unsigned long long	uint64;

	typedef unsigned __int64		Size_T;

	typedef char				ANSICHAR;
	typedef wchar_t			WIDECHAR;
	typedef char16_t			CHAR16;
	typedef char32_t			CHAR32;

};

typedef WindowsTypes            PlatformTypes;


/**
  Platform misc
*/
struct Platform
{
	static void PlatformPreInit();
	static void PlatformInit();

	inline static void MemoryWriteBarrier() { _mm_sfence(); }

	inline static void MemoryReadBarrier() { _mm_lfence(); }

	inline static void MemoryRWBarrier() { _mm_mfence(); }


	static void RequestExit(bool ForceExit);

	static PlatformTypes::int32 ReportCrash(LPEXCEPTION_POINTERS ExceptionInfo);


	inline static PlatformTypes::int32 GetCurrentThreadId()
	{
		return ::GetCurrentThreadId();
	}


	inline static void SetCurrentThreadAffinityMask(PlatformTypes::uint64 AffinityMask)
	{
		::SetThreadAffinityMask(::GetCurrentThread(), (DWORD_PTR)AffinityMask);
	}

	inline static PlatformTypes::int32 TruncToInt(float FNum)
	{
		//SSE Intrinsic
		return _mm_cvt_ss2si(_mm_set_ss(FNum));
	}

	inline static PlatformTypes::int32 NumberOfCores()
	{
		static PlatformTypes::int32 NumOfCores = 0;
		if (NumOfCores == 0)
		{
			SYSTEM_INFO SystemInfo;
			::GetSystemInfo(&SystemInfo);
			NumOfCores = SystemInfo.dwNumberOfProcessors;
		}
		return NumOfCores;
	}

	//Second = 0 will switch to thread
	inline static void Sleep(float Seconds)
	{
		DWORD MicroSeconds = (DWORD)(Seconds * 1000.0);
		if (MicroSeconds == 0)
		{
			::SwitchToThread();
		}
		::Sleep(MicroSeconds);
	}


	inline static bool IsCurrentProcessForeground()
	{
		PlatformTypes::uint32 ForegroundProcess;
		::GetWindowThreadProcessId(GetForegroundWindow(), (DWORD*)&ForegroundProcess);
		return (ForegroundProcess == GetCurrentProcessId());
	}


	inline static void LocalPrintA(const PlatformTypes::ANSICHAR* Message)
	{
		OutputDebugStringA(Message);
	}


	inline static void LocalPrintW(const PlatformTypes::WIDECHAR* Message)
	{
		OutputDebugStringW(Message);
	}


	inline static bool IsDebuggerPresent()
	{
		return ::IsDebuggerPresent();
	}

	inline static void DebugBreak()
	{
		::DebugBreak();
	}

	inline static void GetCurrentDirW(PlatformTypes::WIDECHAR* OutDir, PlatformTypes::uint32 DestSize)
	{
		::GetCurrentDirectoryW(DestSize, OutDir);
	}

	inline static void GetCurrentDirA(PlatformTypes::ANSICHAR* OutDir, PlatformTypes::uint32 DestSize)
	{
		::GetCurrentDirectoryA(DestSize, OutDir);
	}

	static const PlatformTypes::WIDECHAR* GetSystemErrorMessage(PlatformTypes::WIDECHAR* OutBuffer, PlatformTypes::int32 BufferCount, PlatformTypes::int32 Error);
};





/**
  Atomic functions
*/
class WindowsAtomics
{
public:
	static_assert(sizeof(PlatformTypes::int8) == sizeof(char) && alignof(PlatformTypes::int8) == alignof(char), "size of int8 should be 8 bits, same as char type");
	static_assert(sizeof(PlatformTypes::int16) == sizeof(signed short) && alignof(PlatformTypes::int16) == alignof(signed short), "size of int16 should be 16 bits, same as short type");
	//long will be 32 bits in windows using visual studio(IL32P64) 
	static_assert(sizeof(PlatformTypes::int32) == sizeof(long) && alignof(PlatformTypes::int32) == alignof(long), "size of int32 should be 32 bits, same as long type(WINDOWS)");
	static_assert(sizeof(PlatformTypes::int64) == sizeof(long long) && alignof(PlatformTypes::int64) == alignof(long long), "size of int64 should be 64 bits, same as long type");

	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int8 InterlockedIncrement(volatile PlatformTypes::int8* Val)
	{
		return (PlatformTypes::int8)_InterlockedExchangeAdd8((char*)Val, 1) + 1;
	}
	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int16 InterlockedIncrement(volatile PlatformTypes::int16* Val)
	{
		return (PlatformTypes::int16)_InterlockedIncrement16((short*)Val);
	}
	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int32 InterlockedIncrement(volatile PlatformTypes::int32* Val)
	{
		return (PlatformTypes::int32)_InterlockedIncrement((long*)Val);
	}

	/**
       return the INITIAL value of Val
    */
	FORCE_INLINE
	static PlatformTypes::int64 InterlockedIncrement(volatile PlatformTypes::int64* Val)
	{
		return (PlatformTypes::int64)_InterlockedIncrement64((long long*)Val);
	}


	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int8 InterlockedDecrement(volatile PlatformTypes::int8* Val)
	{
		return (PlatformTypes::int8)_InterlockedExchangeAdd8((char*)Val, -1) - 1;
	}
	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int16 InterlockedDecrement(volatile PlatformTypes::int16* Val)
	{
		return (PlatformTypes::int16)_InterlockedDecrement16((short*)Val);
	}
	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int32 InterlockedDecrement(volatile PlatformTypes::int32* Val)
	{
		return (PlatformTypes::int32)_InterlockedDecrement((long*)Val);
	}

	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
		static PlatformTypes::int64 InterlockedDecrement(volatile PlatformTypes::int64* Val)
	{
		return (PlatformTypes::int64)_InterlockedDecrement64((long long*)Val);
	}


	/**
	  return the FINAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int8 InterlockedAdd(volatile PlatformTypes::int8* Val, PlatformTypes::int8 Add)
	{
		return (PlatformTypes::int8)_InterlockedExchangeAdd8((char*)Val, (char)Add);
	}
	/**
	  return the FINAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int16 InterlockedAdd(volatile PlatformTypes::int16* Val, PlatformTypes::int16 Add)
	{
		return (PlatformTypes::int16)_InterlockedExchangeAdd16((short*)Val, (short)Add);
	}
	/**
	  return the FINAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int32 InterlockedAdd(volatile PlatformTypes::int32* Val, PlatformTypes::int32 Add)
	{
		return (PlatformTypes::int32)_InterlockedExchangeAdd((long*)Val, (long)Add);
	}
	/**
	  return the FINAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int64 InterlockedAdd(volatile PlatformTypes::int64* Val, PlatformTypes::int64 Add)
	{
		return (PlatformTypes::int64)_InterlockedExchangeAdd64((long long*)Val, (long long)Add);
	}


	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int8 InterlockedExchange(volatile PlatformTypes::int8* Val, PlatformTypes::int8 Exchange)
	{
		return (PlatformTypes::int8)_InterlockedExchange8((char*)Val, (char)Exchange);
	}
	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int16 InterlockedExchange(volatile PlatformTypes::int16* Val, PlatformTypes::int16 Exchange)
	{
		return (PlatformTypes::int16)_InterlockedExchange16((short*)Val, (short)Exchange);
	}
	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int32 InterlockedExchange(volatile PlatformTypes::int32* Val, PlatformTypes::int32 Exchange)
	{
		return (PlatformTypes::int32)_InterlockedExchange((long*)Val, (long)Exchange);
	}
	/**
	  return the INITIAL value of Val
	*/
	FORCE_INLINE
	static PlatformTypes::int64 InterlockedExchange(volatile PlatformTypes::int64* Val, PlatformTypes::int64 Exchange)
	{
		return (PlatformTypes::int64)_InterlockedExchange64((long long*)Val, (long long)Exchange);
	}
	
	FORCE_INLINE
	static void* InterlockedExchangePtr(void** Dest, void* Exchange)
	{
		return (void*)_InterlockedExchange64((long long*)Dest, (long long)Exchange);
	}


	/**
	  return the INITIAL value of Dest
	*/
	FORCE_INLINE
	static PlatformTypes::int8 InterlockedCompareExchange(volatile PlatformTypes::int8* Dest, PlatformTypes::int8 Exchange, PlatformTypes::int8 Comparend)
	{
		return (PlatformTypes::int8)_InterlockedCompareExchange8((char*)Dest, (char)Exchange, (char)Comparend);
	}
	/**
	  return the INITIAL value of Dest
	*/
	FORCE_INLINE
	static PlatformTypes::int16 InterlockedCompareExchange(volatile PlatformTypes::int16* Dest, PlatformTypes::int16 Exchange, PlatformTypes::int16 Comparend)
	{
		return (PlatformTypes::int16)_InterlockedCompareExchange16((short*)Dest, (short)Exchange, (short)Comparend);
	}
	/**
	  return the INITIAL value of Dest
	*/
	FORCE_INLINE
	static PlatformTypes::int32 InterlockedCompareExchange(volatile PlatformTypes::int32* Dest, PlatformTypes::int32 Exchange, PlatformTypes::int32 Comparend)
	{
		return (PlatformTypes::int32)_InterlockedCompareExchange((long*)Dest, (long)Exchange, (long)Comparend);
	}
	/**
	  return the INITIAL value of Dest
	*/
	FORCE_INLINE
	static PlatformTypes::int64 InterlockedCompareExchange(volatile PlatformTypes::int64* Dest, PlatformTypes::int64 Exchange, PlatformTypes::int64 Comparend)
	{
		return (PlatformTypes::int64)_InterlockedCompareExchange64((long long*)Dest, (long long)Exchange, (long long)Comparend);
	}

	FORCE_INLINE
	static void* InterlockedCompareExchangePtr(void** Dest, void* Exchange, void* Comparend)
	{
		return (void*)_InterlockedCompareExchange64((long long*)Dest, (long long)Exchange, (long long)Comparend);
	}



	FORCE_INLINE
	static PlatformTypes::int8 InterlockedRead(volatile const PlatformTypes::int8* Src)
	{
		return (PlatformTypes::int8)_InterlockedCompareExchange8((char*)Src, 0, 0);
	}

	FORCE_INLINE
		static PlatformTypes::int16 InterlockedRead(volatile const PlatformTypes::int16* Src)
	{
		return (PlatformTypes::int16)_InterlockedCompareExchange16((short*)Src, 0, 0);
	}

	FORCE_INLINE
	static PlatformTypes::int32 InterlockedRead(volatile const PlatformTypes::int32* Src)
	{
		return (PlatformTypes::int32)_InterlockedCompareExchange((long*)Src, 0, 0);
	}

	FORCE_INLINE
	static PlatformTypes::int64 InterlockedRead(volatile const PlatformTypes::int64* Src)
	{
		return (PlatformTypes::int64)_InterlockedCompareExchange64((long long*)Src, 0, 0);
	}


	FORCE_INLINE
	static void InterlockedStore(volatile PlatformTypes::int8* Dest, PlatformTypes::int8 Val)
	{
		_InterlockedExchange8((char*)Dest, (char)Val);
	}

	FORCE_INLINE
	static void InterlockedStore(volatile PlatformTypes::int16* Dest, PlatformTypes::int16 Val)
	{
		_InterlockedExchange16((short*)Dest, (short)Val);
	}

	FORCE_INLINE
	static void InterlockedStore(volatile PlatformTypes::int32* Dest, PlatformTypes::int32 Val)
	{
		_InterlockedExchange((long*)Dest, (long)Val);
	}

	FORCE_INLINE
	static void InterlockedStore(volatile PlatformTypes::int64* Dest, PlatformTypes::int64 Val)
	{
		_InterlockedExchange64((long long*)Dest, (long long)Val);
	}



};

typedef WindowsAtomics          PlatformAtomics;


/**
  Critical Section
*/
class WindowsCriticalSection
{
public:
	FORCE_INLINE
	WindowsCriticalSection()
	{
		InitializeCriticalSection(&CriticalSection);
		SetCriticalSectionSpinCount(&CriticalSection, 4000);
	}

	FORCE_INLINE
	~WindowsCriticalSection()
	{
		DeleteCriticalSection(&CriticalSection);
	}


	FORCE_INLINE
	void Lock()
	{
		if (TryEnterCriticalSection(&CriticalSection) == 0)
		{
			EnterCriticalSection(&CriticalSection);
		}
	}


	FORCE_INLINE
	void UnLock()
	{
		LeaveCriticalSection(&CriticalSection);
	}


	FORCE_INLINE
	bool TryLock()
	{
		return (TryEnterCriticalSection(&CriticalSection)) ? true : false;
	}

	WindowsCriticalSection(const WindowsCriticalSection& Other) = delete;
	WindowsCriticalSection& operator=(const WindowsCriticalSection& Other) = delete;

private:
	CRITICAL_SECTION CriticalSection;
};

typedef WindowsCriticalSection  PlatformCriticalSection;




