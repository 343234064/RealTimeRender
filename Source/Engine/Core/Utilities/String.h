/****************************************
String


*****************************************/
#pragma once

#include "Core/GlobalDefinations.h"
#include "Core/Utilities/DynamicArray.h"
#include "HAL/Chars.h"

#define UNKOWN_CHAR '?'



/**************************
Helper to test if
the char is a valid member of its encoding
****************************/
template <typename Encoding>
FORCE_INLINE
bool IsValidChar(Encoding Char)
{
	return true;
};
template<> FORCE_INLINE bool IsValidChar<ANSICHAR>(ANSICHAR Char) { return Char >= 0x00 && Char <= 0x7F; };



/*
  Convert chars from SrcCharType to DestCharType
  SrcLen: The length of char in SrcChars
  DestMaxLen: The max length that char can be stored in DestChars
  Return nullptr if failed
*/
template <typename DestCharType, typename SrcCharType>
static FORCE_INLINE
typename FuncTrigger<IsCharEncodingCompatible<DestCharType, SrcCharType>::Value, DestCharType*>::Type
ConvertChar(DestCharType* DestChars, int32 DestMaxLen, const SrcCharType* SrcChars, int32 SrcLen)
{
	if (DestMaxLen < SrcLen) return nullptr;
	return (DestCharType*)Memory::Copy(DestChars, SrcChars, SrcLen * sizeof(DestCharType));
}

/*
  Convert chars from SrcCharType to DestCharType
  SrcLen: The length of char in SrcChars
  DestMaxLen: The max length that char can be stored in DestChars
  Return nullptr if failed
*/
template <typename DestCharType, typename SrcCharType>
static FORCE_INLINE
typename FuncTrigger<!IsCharEncodingCompatible<DestCharType, SrcCharType>::Value, DestCharType*>::Type
ConvertChar(DestCharType* DestChars, int32 DestMaxLen, const SrcCharType* SrcChars, int32 SrcLen)
{
	bool CanNotConvertChar = false;

	while (SrcLen)
	{
		//Since the SrcCharType and DestCharType are not compatible, the DestMaxLen should be checked every loop
		if (!DestMaxLen) return nullptr;

		SrcCharType SrcChar = *SrcChars;
		//Test if SrcChar can convert to DestCharType
		if ((SrcCharType)(DestCharType)SrcChar == SrcChar && IsValidChar((DestCharType)SrcChar))
		{
			*DestChars = (DestCharType)SrcChar;
		}
		else
		{
			*DestChars = (DestCharType)UNKOWN_CHAR;
			CanNotConvertChar = true;
		}
		++DestChars;
		++SrcChars;
		--SrcLen;
		--DestMaxLen;
	}

	if (CanNotConvertChar)
	{
		//log
	}

	return DestChars;
}


class String
{
public:
	String() = default;
	String(String&&) = default;
	String(const String&) = default;
	String& operator=(String&&) = default;
	String& operator=(const String&) = default;


	template <typename CharType, typename = typename FuncTrigger<IsCharFixedEncoding<CharType>::Value && IsCharFixedEncoding<TChar>::Value>::Type>
	FORCE_INLINE
	String(const CharType* Src)
	{
		if (Src && *Src)
		{
			int32 SrcLen = PlatformChars::Strlen(Src) + 1;
			Strings.AddUninitialize(SrcLen);
			ConvertChar(Strings.Begin(), Strings.MaxNum(), Src, SrcLen);
		}
	}

	template <typename CharType, typename = typename FuncTrigger<IsCharFixedEncoding<CharType>::Value && IsCharFixedEncoding<TChar>::Value>::Type>
	FORCE_INLINE
	String(const CharType* Src, int32 NumToCopy)
	{
		if (Src && *Src)
		{
			int32 SrcLen = NumToCopy + 1;
			Strings.AddUninitialize(SrcLen);
			ConvertChar(Strings.Begin(), Strings.MaxNum(), Src, SrcLen);
			*(Strings.Begin() + Strings.CurrentNum() - 1) - TEXTS('\0');
		}
	}

	FORCE_INLINE
	String& operator=(const TChar* Other)
	{
		if (Strings.Begin() != Other)
		{
			//if Other == nullptr or null char, just clear the string
			int32 Len = (Other && *Other)? int32(PlatformChars::Strlen(Other) + 1) : 0;
			Strings.ClearElements();
			Strings.AddUninitialize(Len);

			if(Len) Memory::Copy(Strings.Begin(), Other, Len * sizeof(TChar));
		}
	}

	FORCE_INLINE
	TChar& operator[](int32 Index)
	{
		//check
		return Strings[Index];
	}

	FORCE_INLINE
	const TChar& operator[](int32 Index) const
	{
		//check
		return Strings[Index];
	}

	FORCE_INLINE
	Size_T AllocatedSize() const { return Strings.AllocatedSize(); }

	//Clear the string
	FORCE_INLINE
	void Clear() { Strings.ClearElements(); }

	FORCE_INLINE
	bool IsClear() const { return Strings.CurrentNum() < 1; }

	//Clear the string and allocation
	FORCE_INLINE
	void Empty() { return Strings.Empty(); }

	FORCE_INLINE
	bool Shrink() { Strings.Shrink(); }

	FORCE_INLINE
	int32 Len() const { return Strings.CurrentNum() ? Strings.CurrentNum()-1 : 0; }

	FORCE_INLINE
	TChar* operator*()
	{
		return Strings.Begin();
	}

	FORCE_INLINE
	const TChar* operator*() const
	{
		return Strings.Begin();
	}

	FORCE_INLINE
	void Append(const TChar* Src, int32 Num)
	{
		//check
		if (!Num) return;

		int32 CurNum = Strings.CurrentNum();
		Strings.AddUninitialize(Num + (CurNum? 0 : 1));
		TChar* InsertAddress = Strings.Begin() + CurNum - (CurNum ? 1 : 0);

		Memory::CopyAssignItem(InsertAddress, Src, Num);

		*(InsertAddress + Num) = 0;
	}

	template<typename CharType>
	FORCE_INLINE
	typename FuncTrigger<IsCharType<CharType>::Value>::Type
	Append(CharType Char)
	{
		//check
		if (Char != 0)
		{
			int32 InsertIndex = Strings.CurrentNum() ? Strings.CurrentNum() - 1 : 0;
			Strings.AddUninitialize(Strings.CurrentNum() ? 1 : 2);
			Strings[InsertIndex] = Char;
			Strings[InsertIndex + 1] = 0;
		}

	}


	FORCE_INLINE
	String& operator+=(const TChar* Other)
	{
		Append(Other, (int32)PlatformChars::Strlen(Other));
		return *this;
	}

	FORCE_INLINE
	String& operator+=(const String& Other)
	{
		Append(Other.Strings.Begin(), Other.Len());
		return *this;
	}

	FORCE_INLINE
	String& operator+=(const TChar Char)
	{
		Append(Char);
		return *this;
	}

	FORCE_INLINE
	void RemoveAt(int32 Index, int32 Num = 1, bool AllowShrinking = true)
	{
		//TODO: if TChar is not fixed size
		Strings.RemoveAt(Index, Num, AllowShrinking);
	}

	FORCE_INLINE
	void Insert(int32 Index, TChar Char)
	{
		//TODO: if TChar is not fixed size
		if (Char != 0)
		{
			if (Strings.CurrentNum() == 0)
			{
				*this += Char;
			}
			else
			{
				Strings.Insert(Index, Char);
			}
		}
	}

	FORCE_INLINE
	void Insert(int32 Index, const String& Chars)
	{
		//TODO: if TChar is not fixed size
		if (Chars.Len())
		{
			if (Strings.CurrentNum() == 0)
			{
				*this += Chars;
			}
			else
			{
				Strings.Insert(Index, Chars.Len(), Chars.Strings.Begin());
			}
		}
	}



	bool StartWith(const TChar* Prefix, bool IgnoreCase)
	{
		if (IgnoreCase)
		{
			return (Prefix && *Prefix && !PlatformChars::Strnicmp(Strings.Begin(), Prefix, PlatformChars::Strlen(Prefix)));
		}
		else
		{
			return (Prefix && *Prefix && !PlatformChars::Strncmp(Strings.Begin(), Prefix, PlatformChars::Strlen(Prefix)));
		}
	}


	bool StartWith(const String& Prefix, bool IgnoreCase)
	{
		if (IgnoreCase)
		{
			return (Prefix.Len() && !PlatformChars::Strnicmp(Strings.Begin(), *Prefix, Prefix.Len()));
		}
		else
		{
			return (Prefix.Len() && !PlatformChars::Strnicmp(Strings.Begin(), *Prefix, Prefix.Len()));
		}
	}


	bool EndWith(const TChar* Suffix, bool IgnoreCase)
	{
		if (!(Suffix && *Suffix)) return false;

		int32 Offset = Len() - PlatformChars::Strlen(Suffix);
		
		if (Offset < 0) return false;
		if (IgnoreCase)
		{
			return !PlatformChars::Stricmp(Strings.Begin() + Offset, Suffix);
		}
		else
		{
			return !PlatformChars::Strcmp(Strings.Begin() + Offset, Suffix);
		}
	}

	bool EndWith(const String& Suffix, bool IgnoreCase)
	{
		if (!Suffix.Len()) return false;

		int32 Offset = Len() - Suffix.Len();

		if (Offset < 0) return false;
		if (IgnoreCase)
		{
			return !PlatformChars::Stricmp(Strings.Begin() + Offset, *Suffix);
		}
		else
		{
			return !PlatformChars::Strcmp(Strings.Begin() + Offset, *Suffix);
		}
	}

	bool RemovePrefix(const TChar* Prefix, bool IgnoreCase)
	{
		if (StartWith(Prefix, IgnoreCase))
		{
			RemoveAt(0, PlatformChars::Strlen(Prefix));
			return true;
		}

		return false;
	}

	bool RemovePrefix(const String& Prefix, bool IgnoreCase)
	{
		if (StartWith(Prefix, IgnoreCase))
		{
			RemoveAt(0, Prefix.Len());
			return true;
		}

		return false;
	}

	bool RemoveSuffix(const TChar* Suffix, bool IgnoreCase)
	{
		if (EndWith(Suffix, IgnoreCase))
		{
			int32 SuffixLen = PlatformChars::Strlen(Suffix);
			RemoveAt(Len() - SuffixLen, SuffixLen);
			return true;
		}

		return false;
	}

	bool RemoveSuffix(const String& Suffix, bool IgnoreCase)
	{
		if (EndWith(Suffix, IgnoreCase))
		{
			RemoveAt(Len() - Suffix.Len(), Suffix.Len());
			return true;
		}

		return false;
	}

protected:
	Array<TChar> Strings;
};