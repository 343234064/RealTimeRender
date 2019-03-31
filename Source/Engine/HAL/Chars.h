#pragma once

#include "HAL/Platform.h"


//==============================
//Text define
//==============================
#if PLATFORM_TEXT_IS_WCHAR
#  define TEXTS(x) L ## x//wchar_t
#elif PLATFORM_TEXT_IS_CHAR32
#  define TEXTS(x) U ## x//char32_t, utf-32
/* Currently not support
#if PLATFORM_TEXT_IS_CHAR16
#  define TEXTS(x) u ## x//char16_t, utf-16
*/
#else 
#  error Text macro is not defined
#endif



//==============================
//Chars
//==============================
namespace Chars
{
#pragma warning(push)
#pragma warning(disable: 4244)
	template <typename WCharType> inline WCharType ToUpper(WCharType Char)
	{

		if (Char < 128)
		{

			return ::toupper((ANSICHAR)Char);
		}

		return ::towupper(Char);
	}


	template <> inline PlatformTypes::CHAR32 ToUpper(PlatformTypes::CHAR32 Char)
	{
		return ::towupper(Char);
	}

	template <typename WCharType> inline WCharType ToLower(WCharType Char)
	{

		if (Char < 128)
		{
			return ::tolower((ANSICHAR)Char);
		}

		return ::towlower(Char);
	}

	template <> inline PlatformTypes::CHAR32 ToLower(PlatformTypes::CHAR32 Char)
	{
		return ::towlower(Char);
	}
#pragma warning(pop)




	template <typename WCharType> inline bool IsUpper(WCharType Char) { return ::iswupper(Char) != 0; }
	template <typename WCharType> inline bool IsLower(WCharType Char) { return ::iswlower(Char) != 0; }
	template <typename WCharType> inline bool IsAlpha(WCharType Char) { return ::iswalpha(Char) != 0; }
	template <typename WCharType> inline bool IsGraph(WCharType Char) { return ::iswgraph(Char) != 0; }
	template <typename WCharType> inline bool IsPrint(WCharType Char) { return ::iswprint(Char) != 0; }
	template <typename WCharType> inline bool IsPunct(WCharType Char) { return ::iswpunct(Char) != 0; }
	template <typename WCharType> inline bool IsAlnum(WCharType Char) { return ::iswalnum(Char) != 0; }
	template <typename WCharType> inline bool IsDigit(WCharType Char) { return ::iswdigit(Char) != 0; }
	template <typename WCharType> inline bool IsHexDigit(WCharType Char) { return ::iswxdigit(Char) != 0; }
	template <typename WCharType> inline bool IsWhitespace(WCharType Char) { return ::iswspace(Char) != 0; }


	template <> inline ANSICHAR ToUpper(ANSICHAR Char) { return (ANSICHAR)::toupper(Char); }
	template <> inline ANSICHAR ToLower(ANSICHAR Char) { return (ANSICHAR)::tolower(Char); }
	template <> inline bool IsUpper(ANSICHAR Char) { return ::isupper((unsigned char)Char) != 0; }
	template <> inline bool IsLower(ANSICHAR Char) { return ::islower((unsigned char)Char) != 0; }
	template <> inline bool IsAlpha(ANSICHAR Char) { return ::isalpha((unsigned char)Char) != 0; }
	template <> inline bool IsGraph(ANSICHAR Char) { return ::isgraph((unsigned char)Char) != 0; }
	template <> inline bool IsPrint(ANSICHAR Char) { return ::isprint((unsigned char)Char) != 0; }
	template <> inline bool IsPunct(ANSICHAR Char) { return ::ispunct((unsigned char)Char) != 0; }
	template <> inline bool IsAlnum(ANSICHAR Char) { return ::isalnum((unsigned char)Char) != 0; }
	template <> inline bool IsDigit(ANSICHAR Char) { return ::isdigit((unsigned char)Char) != 0; }
	template <> inline bool IsHexDigit(ANSICHAR Char) { return ::isxdigit((unsigned char)Char) != 0; }
	template <> inline bool IsWhitespace(ANSICHAR Char) { return ::isspace((unsigned char)Char) != 0; }

}



#if PLATFORM_WINDOWS
#include "HAL/Platforms/Windows/WindowsChars.h"
#elif PLATFORM_LINUX
#include "HAL/Platforms/Linux/LinuxChars.h"
#elif PLATFORM_MAC
#include "HAL/Platforms/Mac/MacChars.h"
#else
#error Unknown platform
#endif



