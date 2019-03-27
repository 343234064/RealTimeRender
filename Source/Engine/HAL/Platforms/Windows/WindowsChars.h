#pragma once


#include "HAL/Platform.h"

#define _UNICODE
#include <tchar.h>
#include <stdlib.h>


/**
  Chars
  TODO: CHAR16/CHAR32 NOT COMPLETED
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

	static PlatformTypes::CHAR16* Strcpy(PlatformTypes::CHAR16* Dest, PlatformTypes::Size_T DestNum, const PlatformTypes::CHAR16* Src)
	{
		PlatformTypes::CHAR16* Buf = Dest;
		while (*Src && --DestNum)
		{
			*Buf++ = *Src++;
		}
		*Buf = 0;

		return Dest;
	}

	static PlatformTypes::CHAR32* Strcpy(PlatformTypes::CHAR32* Dest, PlatformTypes::Size_T DestNum, const PlatformTypes::CHAR32* Src)
	{
		PlatformTypes::CHAR32* Buf = Dest;
		while (*Src && --DestNum)
		{
			*Buf++ = *Src++;
		}
		*Buf = 0;

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

	static PlatformTypes::CHAR16* Strncpy(PlatformTypes::CHAR16* Dest, const PlatformTypes::CHAR16* Src, PlatformTypes::Size_T MaxNumToCopy)
	{
		PlatformTypes::CHAR16* Buf = Dest;
		bool FillZero = false;
		while (--MaxNumToCopy)
		{
			if (FillZero)
			{
				*Buf++ = 0;
			}
			else
			{
				if (*Src == 0)
					FillZero = true;

				*Buf++ = *Src++;
			}
		}
		*Buf = 0;

		return Dest;
	}

	static PlatformTypes::CHAR32* Strncpy(PlatformTypes::CHAR32* Dest, const PlatformTypes::CHAR32* Src, PlatformTypes::Size_T MaxNumToCopy)
	{
		PlatformTypes::CHAR32* Buf = Dest;
		bool FillZero = false;
		while (--MaxNumToCopy)
		{
			if (FillZero)
			{
				*Buf++ = 0;
			}
			else
			{
				if (*Src == 0)
					FillZero = true;

				*Buf++ = *Src++;
			}
		}
		*Buf = 0;

		return Dest;
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

	static PlatformTypes::CHAR16* Strcat(PlatformTypes::CHAR16* Dest, PlatformTypes::Size_T DestNum, const PlatformTypes::CHAR16* Src)
	{
		PlatformTypes::CHAR16* Buf = Dest;

		while (*Buf != 0 && DestNum > 1)
		{
			Buf++;
			DestNum--;
		}
		while (*Src != 0 && DestNum > 1)
		{
			*Buf++ = *Src++;
			DestNum--;
		}
		*Buf = 0;

		return Dest;
	}

	static PlatformTypes::CHAR32* Strcat(PlatformTypes::CHAR32* Dest, PlatformTypes::Size_T DestNum, const PlatformTypes::CHAR32* Src)
	{
		PlatformTypes::CHAR32* Buf = Dest;

		while (*Buf != 0 && DestNum > 1)
		{
			Buf++;
			DestNum--;
		}
		while (*Src != 0 && DestNum > 1)
		{
			*Buf++ = *Src++;
			DestNum--;
		}
		*Buf = 0;

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

	static PlatformTypes::int32 Strcmp(const PlatformTypes::CHAR16* Dest1, const PlatformTypes::CHAR16* Dest2)
	{
		for (; *Dest1 || *Dest2; Dest1++, Dest2++)
		{
			PlatformTypes::CHAR16 A = *Dest1, B = *Dest2;
			if (A != B) return A - B;
		}
		return 0;
	}

	static PlatformTypes::int32 Strcmp(const PlatformTypes::CHAR32* Dest1, const PlatformTypes::CHAR32* Dest2)
	{
		for (; *Dest1 || *Dest2; Dest1++, Dest2++)
		{
			PlatformTypes::CHAR32 A = *Dest1, B = *Dest2;
			if (A != B) return A - B;
		}
		return 0;
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

	static PlatformTypes::int32 Strncmp(const PlatformTypes::CHAR16* Dest1, const PlatformTypes::CHAR16* Dest2, PlatformTypes::Size_T Num)
	{
		for (; *Dest1 || *Dest2 && Num; Dest1++, Dest2++, Num--)
		{
			PlatformTypes::CHAR16 A = *Dest1, B = *Dest2;
			if (A != B) return A - B;
		}
		return 0;
	}

	static PlatformTypes::int32 Strncmp(const PlatformTypes::CHAR32* Dest1, const PlatformTypes::CHAR32* Dest2, PlatformTypes::Size_T Num)
	{
		for (; *Dest1 || *Dest2 && Num; Dest1++, Dest2++, Num--)
		{

			PlatformTypes::CHAR32 A = *Dest1, B = *Dest2;
			if (A != B) return A - B;
		}
		return 0;
	}


	template<typename CharTypeA, typename CharTypeB>
	static typename FuncTrigger<IsCharComparisonCompatible<CharTypeA, CharTypeB>::Value, PlatformTypes::int32>::Type
	Stricmp(const CharTypeA* Dest1, const CharTypeB* Dest2)
	{
		for (; *Dest1 || *Dest2; Dest1++, Dest2++)
		{
			if (*Dest1 == *Dest2) continue;

			CharTypeA Tmp1 = ToLower(*Dest1);
			CharTypeB Tmp2 = ToLower(*Dest2);
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


	static PlatformTypes::int32 Strnicmp(const PlatformTypes::CHAR16* Dest1, const PlatformTypes::CHAR16* Dest2, PlatformTypes::Size_T Num)
	{
		for (; *Dest1 || *Dest2 && Num; Dest1++, Dest2++, Num--)
		{
			PlatformTypes::CHAR16 A = ToUpper(*Dest1), B = ToUpper(*Dest2);
			if (A != B) return A - B;
		}
		return 0;
	}

	static PlatformTypes::int32 Strnicmp(const PlatformTypes::CHAR32* Dest1, const PlatformTypes::CHAR32* Dest2, PlatformTypes::Size_T Num)
	{
		for (; *Dest1 || *Dest2 && Num; Dest1++, Dest2++, Num--)
		{
			PlatformTypes::CHAR32 A = ToUpper(*Dest1), B = ToUpper(*Dest2);
			if (A != B) return A - B;
		}
		return 0;
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
	static PlatformTypes::Size_T Strlen(const PlatformTypes::CHAR16* Dest)
	{
		//TODO:CHAR16 is not fixed size
		PlatformTypes::int32 Len = -1;

		do
		{
			Len++;
		} while (*Dest++);

		return Len;
	}

	FORCE_INLINE
	static PlatformTypes::Size_T Strlen(const PlatformTypes::CHAR32* Dest)
	{
		PlatformTypes::int32 Len = -1;

		do
		{
			Len++;
		} while (*Dest++);

		return Len;
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

	/*
	FORCE_INLINE
	static PlatformTypes::int32 Atoi(const PlatformTypes::CHAR16* Dest)
	{
		return Strtoi(Dest, nullptr, 10);
	}

	FORCE_INLINE
	static PlatformTypes::int32 Atoi(const PlatformTypes::CHAR32* Dest)
	{
		return Strtoi(Dest, nullptr, 10);
	}
	*/


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

	/*
	FORCE_INLINE
	static PlatformTypes::int64 Atoi(const PlatformTypes::CHAR16* Dest)
	{
		return Strtoi64(Dest, nullptr, 10);
	}

	FORCE_INLINE
	static PlatformTypes::int64 Atoi(const PlatformTypes::CHAR32* Dest)
	{
		return Strtoi64(Dest, nullptr, 10);
	}
	*/


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

	/*
	FORCE_INLINE
	static PlatformTypes::int32 Strtoi(const PlatformTypes::CHAR16* Dest, PlatformTypes::CHAR16** End, PlatformTypes::int32 Base)
	{

	}

	static PlatformTypes::int32 Strtoi(const PlatformTypes::CHAR32* Dest, PlatformTypes::CHAR32** End, PlatformTypes::int32 Base)
	{

	}*/


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



};

typedef WindowsChars           PlatformChars;

