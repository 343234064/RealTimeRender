#pragma once


#include "HAL/Platform.h"


#define _UNICODE
#include <tchar.h>
#include <stdlib.h>


#define LINE_TERMINATOR TEXTS("\r\n")
#define LINE_TERMINATOR_ANSI TEXTS("\r\n")




/**
  Chars
  WIDECHAR = wchar_t
*/
class WindowsChars
{
public:
	FORCE_INLINE
		static PlatformTypes::WIDECHAR* Strcpy(PlatformTypes::WIDECHAR* Dest, PlatformTypes::Size_T DestNum, const PlatformTypes::WIDECHAR* Src)
	{
		_tcscpy_s(Dest, DestNum, Src);
		return Dest;
	}

	FORCE_INLINE
		static PlatformTypes::ANSICHAR* Strcpy(PlatformTypes::ANSICHAR* Dest, PlatformTypes::Size_T DestNum, const PlatformTypes::ANSICHAR* Src)
	{
		strcpy_s(Dest, DestNum, Src);
		return Dest;
	}


	FORCE_INLINE
		static PlatformTypes::WIDECHAR* Strncpy(PlatformTypes::WIDECHAR* Dest, const PlatformTypes::WIDECHAR* Src, PlatformTypes::Size_T MaxNumToCopy)
	{
		_tcsncpy_s(Dest, MaxNumToCopy, Src, MaxNumToCopy - 1);
		return Dest;
	}

	FORCE_INLINE
		static void Strncpy(PlatformTypes::ANSICHAR* Dest, const PlatformTypes::ANSICHAR* Src, PlatformTypes::Size_T MaxNumToCopy)
	{
		strncpy_s(Dest, MaxNumToCopy, Src, MaxNumToCopy - 1);
	}


	FORCE_INLINE
		static PlatformTypes::WIDECHAR* Strcat(PlatformTypes::WIDECHAR* Dest, PlatformTypes::Size_T DestNum, const PlatformTypes::WIDECHAR* Src)
	{
		_tcscat_s(Dest, DestNum, Src);
		return Dest;
	}

	FORCE_INLINE
		static PlatformTypes::ANSICHAR* Strcat(PlatformTypes::ANSICHAR* Dest, PlatformTypes::Size_T DestNum, const PlatformTypes::ANSICHAR* Src)
	{
		strcat_s(Dest, DestNum, Src);
		return Dest;
	}

	FORCE_INLINE 
		static const PlatformTypes::WIDECHAR* Strchr(const PlatformTypes::WIDECHAR* String, PlatformTypes::WIDECHAR C)
	{
		return _tcschr(String, C);
	}

	FORCE_INLINE
		static const PlatformTypes::ANSICHAR* Strchr(const PlatformTypes::ANSICHAR* String, PlatformTypes::ANSICHAR C)
	{
		return strchr(String, C);
	}

	
	FORCE_INLINE
	static PlatformTypes::WIDECHAR* Strncat(PlatformTypes::WIDECHAR* Dest, const PlatformTypes::WIDECHAR* Src, PlatformTypes::int32 MaxLen)
	{
		PlatformTypes::Size_T Len = Strlen(Dest);
		PlatformTypes::WIDECHAR* NewDest = Dest + Len;
		if ((MaxLen -= Len) > 0)
		{
			Strncpy(NewDest, Src, MaxLen);
		}
		return Dest;
	}

	FORCE_INLINE
	static PlatformTypes::ANSICHAR* Strncat(PlatformTypes::ANSICHAR* Dest, const PlatformTypes::ANSICHAR* Src, PlatformTypes::int32 MaxLen)
	{
		PlatformTypes::Size_T Len = Strlen(Dest);
		PlatformTypes::ANSICHAR* NewDest = Dest + Len;
		if ((MaxLen -= Len) > 0)
		{
			Strncpy(NewDest, Src, MaxLen);
		}
		return Dest;
	}


	FORCE_INLINE
		static PlatformTypes::int32 Strcmp(const PlatformTypes::WIDECHAR* Dest1, const PlatformTypes::WIDECHAR* Dest2)
	{
		return _tcscmp(Dest1, Dest2);
	}

	FORCE_INLINE
		static PlatformTypes::int32 Strcmp(const PlatformTypes::ANSICHAR* Dest1, const PlatformTypes::ANSICHAR* Dest2)
	{
		return strcmp(Dest1, Dest2);
	}



	FORCE_INLINE
		static PlatformTypes::int32 Strncmp(const PlatformTypes::WIDECHAR* Dest1, const PlatformTypes::WIDECHAR* Dest2, PlatformTypes::Size_T Num)
	{
		return _tcsncmp(Dest1, Dest2, Num);
	}

	FORCE_INLINE
		static PlatformTypes::int32 Strncmp(const PlatformTypes::ANSICHAR* Dest1, const PlatformTypes::ANSICHAR* Dest2, PlatformTypes::Size_T Num)
	{
		return strncmp(Dest1, Dest2, Num);
	}


	template<typename CharTypeA, typename CharTypeB>
	static typename FuncTrigger<IsCharComparisonCompatible<CharTypeA, CharTypeB>::Value, PlatformTypes::int32>::Type
		Stricmp(const CharTypeA* Dest1, const CharTypeB* Dest2)
	{
		for (; *Dest1 || *Dest2; Dest1++, Dest2++)
		{
			if (*Dest1 == *Dest2) continue;

			CharTypeA Tmp1 = Chars::ToLower(*Dest1);
			CharTypeB Tmp2 = Chars::ToLower(*Dest2);
			if (Tmp1 != Tmp2)
			{
				return *Dest1 - *Dest2;
			}
		}

		return 0;
	}

	FORCE_INLINE
		static PlatformTypes::int32 Stricmp(const PlatformTypes::ANSICHAR* Dest1, const PlatformTypes::ANSICHAR* Dest2)
	{
		return _stricmp(Dest1, Dest2);
	}


	FORCE_INLINE
		static PlatformTypes::int32 Strnicmp(const PlatformTypes::WIDECHAR* Dest1, const PlatformTypes::WIDECHAR* Dest2, PlatformTypes::Size_T Num)
	{
		return _tcsnicmp(Dest1, Dest2, Num);
	}

	FORCE_INLINE
		static PlatformTypes::int32 Strnicmp(const PlatformTypes::ANSICHAR* Dest1, const PlatformTypes::ANSICHAR* Dest2, PlatformTypes::Size_T Num)
	{
		return _strnicmp(Dest1, Dest2, Num);
	}


	FORCE_INLINE
		static PlatformTypes::Size_T Strlen(const PlatformTypes::WIDECHAR* Dest)
	{
		return _tcslen(Dest);
	}

	FORCE_INLINE
		static PlatformTypes::Size_T Strlen(const PlatformTypes::ANSICHAR* Dest)
	{
		return strlen(Dest);
	}


	FORCE_INLINE
		static PlatformTypes::int32 Atoi(const PlatformTypes::WIDECHAR* Dest)
	{
		return _tstoi(Dest);
	}

	FORCE_INLINE
		static PlatformTypes::int32 Atoi(const PlatformTypes::ANSICHAR* Dest)
	{
		return atoi(Dest);
	}


	FORCE_INLINE
		static PlatformTypes::int64 Atoi64(const PlatformTypes::WIDECHAR* Dest)
	{
		return _tstoi64(Dest);
	}

	FORCE_INLINE
		static PlatformTypes::int64 Atoi64(const PlatformTypes::ANSICHAR* Dest)
	{
		return _strtoi64(Dest, nullptr, 10);
	}


	FORCE_INLINE
		static double Atof(const PlatformTypes::WIDECHAR* Dest)
	{
		return _tstof(Dest);
	}

	FORCE_INLINE
		static double Atof(const PlatformTypes::ANSICHAR* Dest)
	{
		return atof(Dest);
	}


	FORCE_INLINE
		static PlatformTypes::int32 Strtoi(const PlatformTypes::WIDECHAR* Dest, PlatformTypes::WIDECHAR** End, PlatformTypes::int32 Base)
	{
		return _tcstoul(Dest, End, Base);
	}

	FORCE_INLINE
		static PlatformTypes::int32 Strtoi(const PlatformTypes::ANSICHAR* Dest, PlatformTypes::ANSICHAR** End, PlatformTypes::int32 Base)
	{
		return strtol(Dest, End, Base);
	}



	FORCE_INLINE
		static PlatformTypes::int64 Strtoi64(const PlatformTypes::WIDECHAR* Dest, PlatformTypes::WIDECHAR** End, PlatformTypes::int32 Base)
	{
		return _tcstoi64(Dest, End, Base);
	}

	FORCE_INLINE
		static PlatformTypes::int64 Strtoi64(const PlatformTypes::ANSICHAR* Dest, PlatformTypes::ANSICHAR** End, PlatformTypes::int32 Base)
	{
		return _strtoi64(Dest, End, Base);
	}


	FORCE_INLINE
		static PlatformTypes::WIDECHAR* Strtok(PlatformTypes::WIDECHAR* Token, const PlatformTypes::WIDECHAR* Delim, PlatformTypes::WIDECHAR** Context)
	{
		return _tcstok_s(Token, Delim, Context);
	}

	FORCE_INLINE
		static PlatformTypes::ANSICHAR* Strtok(PlatformTypes::ANSICHAR* Token, const PlatformTypes::ANSICHAR* Delim, PlatformTypes::ANSICHAR** Context)
	{
		return strtok_s(Token, Delim, Context);
	}



	FORCE_INLINE
		static const PlatformTypes::WIDECHAR* Strstr(const PlatformTypes::WIDECHAR* Dest, const PlatformTypes::WIDECHAR* Find)
	{
		return _tcsstr(Dest, Find);
	}

	FORCE_INLINE
		static const PlatformTypes::ANSICHAR* Strstr(const PlatformTypes::ANSICHAR* Dest, const PlatformTypes::ANSICHAR* Find)
	{
		return strstr(Dest, Find);
	}


	template <typename CharType>
	static const CharType* Stristr(const CharType* Dest, const CharType* Find)
	{
		if (Dest == nullptr || Find == nullptr || *Find == 0) return nullptr;

		//Ignore the first letter 
		int32 FindLength = (int32)WindowsChars::Strlen(Find) - 1;
		
		CharType FindFirstChar = Chars::ToUpper(*Find++);
		CharType DestChar = *Dest++;

		while (DestChar)
		{
			DestChar = Chars::ToUpper(DestChar);
			if (DestChar == FindFirstChar && !WindowsChars::Strnicmp(Dest, Find, FindLength))
			{
				return --Dest;
			}

			DestChar = *Dest++;
		}

		return nullptr;
	}


	FORCE_INLINE 
	static PlatformTypes::int32 VarArgSprintf(PlatformTypes::WIDECHAR* Buffer, Size_T BufferCount, PlatformTypes::int32 MaxCount, const PlatformTypes::WIDECHAR* Format, va_list ArgsPtr)
	{
		return _vsntprintf_s(Buffer, BufferCount, MaxCount, Format, ArgsPtr);
	}

	FORCE_INLINE
	static PlatformTypes::int32 VarArgSprintf(PlatformTypes::ANSICHAR* Buffer, Size_T BufferCount, PlatformTypes::int32 MaxCount,  const PlatformTypes::ANSICHAR* Format, va_list ArgsPtr)
	{
		return _vsnprintf_s(Buffer, BufferCount, MaxCount, Format, ArgsPtr);
	}



	static PlatformTypes::int32 Sprintf(PlatformTypes::WIDECHAR* Buffer, Size_T BufferCount, const PlatformTypes::WIDECHAR* Format, ...)
	{
		PlatformTypes::int32 Result = 0;
		va_list ArgPtr; 
		va_start(ArgPtr, Format);
		Result = WindowsChars::VarArgSprintf(Buffer, (PlatformTypes::int32)BufferCount, (PlatformTypes::int32)BufferCount-1, Format, ArgPtr);
		va_end(ArgPtr); 
		if (Result >= BufferCount) Result = -1; 
		return Result;
	}

	static PlatformTypes::int32 Sprintf(PlatformTypes::ANSICHAR* Buffer, Size_T BufferCount, const PlatformTypes::ANSICHAR* Format, ...)
	{
		PlatformTypes::int32 Result = 0;
		va_list ArgPtr;
		va_start(ArgPtr, Format);
		Result = WindowsChars::VarArgSprintf(Buffer, (PlatformTypes::int32)BufferCount, (PlatformTypes::int32)BufferCount - 1, Format, ArgPtr);
		va_end(ArgPtr);
		if (Result >= BufferCount) Result = -1;
		return Result;
	}


};

typedef WindowsChars           PlatformChars;



