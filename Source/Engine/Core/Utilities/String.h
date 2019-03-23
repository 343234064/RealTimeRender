/****************************************
String


*****************************************/
#pragma once

#include "Core/GlobalDefinations.h"
#include "Core/Utilities/DynamicArray.h"


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


protected:
	Array<TChar> Strings;
};