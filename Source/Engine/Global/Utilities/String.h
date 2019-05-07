/****************************************
String


*****************************************/
#pragma once

#include "Global/Utilities/DynamicArray.h"
#include "Global/Utilities/Misc.h"
#include "HAL/Chars.h"
#include "Global/Utilities/Assertion.h"


#define UNKOWN_CHAR '?'
#define PRINTF_BUFFER_INIT_SIZE 512


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

//TODO:Support for unfixed-encoding char type, e.g.:char16_t
//using char16_t may work fine if the character size is always 2 bytes
class String
{
public:
	String() = default;
	String(String&&) = default;
	String(const String&) = default;
	String& operator=(String&&) = default;
	String& operator=(const String&) = default;

	//template <typename CharType, typename = typename FuncTrigger<IsCharFixedEncoding<CharType>::Value && IsCharFixedEncoding<TChar>::Value>::Type>
	template <typename CharType, typename = typename FuncTrigger<IsCharType<CharType>::Value>::Type>
	FORCE_INLINE
	String(const CharType* Src)
	{
		if (Src && *Src)
		{
			int32 SrcLen = (int32)PlatformChars::Strlen(Src) + 1;
			Strings.AddUninitialize(SrcLen);
			ConvertChar(Strings.Begin(), Strings.MaxNum(), Src, SrcLen);
			*(Strings.Begin() + Strings.CurrentNum() - 1) = TEXTS('\0');
		}
	}

	//template <typename CharType, typename = typename FuncTrigger<IsCharFixedEncoding<CharType>::Value && IsCharFixedEncoding<TChar>::Value>::Type>
	template <typename CharType, typename = typename FuncTrigger<IsCharType<CharType>::Value>::Type>
	FORCE_INLINE
	String(const CharType* Src, int32 NumToCopy)
	{
		if (Src && *Src)
		{
			int32 SrcLen = NumToCopy + 1;
			Strings.AddUninitialize(SrcLen);
			ConvertChar(Strings.Begin(), Strings.MaxNum(), Src, SrcLen);
			*(Strings.Begin() + Strings.CurrentNum() - 1) = TEXTS('\0');
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

		return *this;
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
	bool IsClear() const { return Strings.CurrentNum() <= 1; }

	FORCE_INLINE
	uint32 SizePerElement() const { return Strings.TypeSize(); }

	//Clear the string and allocation
	FORCE_INLINE
	void Empty() { return Strings.Empty(); }

	FORCE_INLINE
	bool Shrink() { Strings.Shrink(); }

	//Return the length of strings except for '\0'
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
		Strings.AddUninitialize(CurNum > 0 ? Num : Num + 1);

		TChar* InsertAddress = Strings.Begin() + (CurNum > 0 ? CurNum - 1 : 0);
		
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
			//TODO: if CharType is not fixed size
			int32 InsertIndex = Strings.CurrentNum() ? Strings.CurrentNum() - 1 : 0;
			int32 InsertCount = Strings.CurrentNum() ? 1 : 2;
			Strings.AddUninitialize(InsertCount);
			Strings[InsertIndex] = Char;
			Strings[InsertIndex + 1] = 0;
		}

	}

	void AppendPath(const TChar* Path, int32 PathLen)
	{
		int32 DestLen = Strings.CurrentNum();
		if (PathLen == 0 || *Path == 0)
		{
			if (DestLen > 1 && Strings[DestLen - 2] != TEXTS('/') && Strings[DestLen - 2] != TEXTS('\\'))
			{
				Strings[DestLen - 1] = TEXTS('/');
				Strings.Add(TEXTS('\0'));
			}
		}
		else
		{
			if (DestLen)
			{
				if (DestLen > 1 && Strings[DestLen - 2] != TEXTS('/') && Strings[DestLen - 2] != TEXTS('\\') && *Path != TEXTS('/'))
				{
					Strings[DestLen - 1] = TEXTS('/');
				}
				else
				{
					Strings.Pop(false);
				}
			}

			Strings.AddUninitialize(1);
			Append(Path, PathLen);
		}
	}

	void AppendInt(int32 Number)
	{
		
		const TChar* NumberList[11] = { TEXTS("0"), TEXTS("1"), TEXTS("2"), TEXTS("3"), TEXTS("4"), 
			                            TEXTS("5"), TEXTS("6"), TEXTS("7"), TEXTS("8"), TEXTS("9"),
			                            TEXTS("-") };
		//If casting -2147483648 to int32 will get error result
		//So cast it to int64 to avoid this error
		int64 Num = Number;
		TChar Buffer[16];
		TChar BufferIndex = 16;

		bool IsNagative = false;
		if (Number < 0)
		{
			IsNagative = true;
			Num = -Num;
		}

		Buffer[--BufferIndex] = 0;

		do
		{
			Buffer[--BufferIndex] = *NumberList[Num % 10];
			Num /= 10;

		} while (Num);

		if (IsNagative)
		{
			Buffer[--BufferIndex] = *NumberList[10];
		}

		*this += (Buffer + BufferIndex);
	}


	void AppendFloat(double FNumber, const int32 MinFractionalDigits = 2)
	{
		//Avoid casting -0.0
		if (FNumber == double(-0.0)) FNumber = 0;

		//Conver to String
		String BufferString = String::Sprintf(TEXTS("%f"), FNumber);
		if (!BufferString.IsNumeric())
		{
			*this += BufferString;
			return;
		}

		//Remove trailing zeros, -1.2000 -> -1.2
		int32 ZeroIndex = -1;
		int32 DotIndex = -1;
		int32 BufferLen = BufferString.Len();
		for (int32 Index = BufferLen - 1; Index >= 0; Index--)
		{
			const TChar Char = BufferString[Index];
			if (Char == TEXTS('.'))
			{
				DotIndex = Index;
				ZeroIndex = std::max(ZeroIndex, DotIndex);
				break;
			}

			if (ZeroIndex == -1 && Char != TEXTS('0'))
				ZeroIndex = Index + 1;
		}
		BufferString.RemoveAt(ZeroIndex, BufferLen - ZeroIndex, false);

		//Handle the minimal fractional digits
		if (MinFractionalDigits)
		{
			if (DotIndex == ZeroIndex)
				BufferString.Append(TEXTS('.'));

			const int32 FractionalDigitsNum = BufferString.Len() - DotIndex - 1;
			const int32 DigitsToAdd = MinFractionalDigits - FractionalDigitsNum;
			if (DigitsToAdd > 0)
			{
				for (int32 i = 0; i < DigitsToAdd; i++)
					BufferString.Append(TEXTS('0'));
			}
			else if (DigitsToAdd < 0)
			{
			    const int32 DigitsToSub = -DigitsToAdd;
			    BufferString.RemoveAt(DotIndex + MinFractionalDigits + 1, DigitsToSub, false);
			}
			
		}

		*this += BufferString;
		return;
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

	
	void RemoveAllMatch(const TChar ToRemove)
	{
		Strings.RemoveAllMatch(ToRemove);
		Strings[Strings.CurrentNum() - 1] = 0;
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

		int32 Offset = Len() - (int32)PlatformChars::Strlen(Suffix);
		
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
			RemoveAt(0, (int32)PlatformChars::Strlen(Prefix));
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
			int32 SuffixLen = (int32)PlatformChars::Strlen(Suffix);
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


	//Get a given Num of characters(including '\0') from the start of string 
	FORCE_INLINE
	String Front(int32 Num)
	{
		return String(**this, Clamp(Num, 0, Len()));
	}

	//Get a given Num of characters(including '\0') from the end of string 
	FORCE_INLINE
	String End(int32 Num)
	{
		int32 Length = Len();
		return String(**this + Length - Clamp(Num, 0, Length));
	}

	//Get the characters(including '\0') from the given range [StartIndex, EndIndex)
	FORCE_INLINE
	String Range(int32 StartIndex, int32 EndIndex)
	{
		if (EndIndex <= 0) EndIndex = Len();

		int32 Length = Len();
		StartIndex = Clamp(StartIndex, 0, Length);
		EndIndex = Clamp(EndIndex, StartIndex, Length);

		return String(**this + StartIndex, EndIndex - StartIndex);
	}

	//Find the substring from the start, and return the index to the substring first found
	//SearchIndex: The start point to begin searching
	//Return -1 if failed to search
	int32 Find(const TChar* SubString, int32 SearchIndex, bool IgnoreCase = true)
	{
		//check substring == null **this
		const TChar* Begin = **this + Clamp(SearchIndex, 0, Len());
		if (*Begin)
		{
			const TChar* Found = IgnoreCase ? PlatformChars::Stristr(Begin, SubString) : PlatformChars::Strstr(Begin, SubString);
			
			return Found ? (int32)(Found - **this) : -1;
		}

		return -1;
	}

	//Find the substring from the end, and return the index to the substring first found
    //SearchIndex: The start point to begin searching
    //Return -1 if failed to search
	int32 FindFromEnd(const TChar* SubString, int32 SearchIndex, bool IgnoreCase = true)
	{
		//check substring == null **this

		//Ignore the first letter 
		int32 SubLength = (int32)PlatformChars::Strlen(SubString) - 1;
		int32 DestLength = Len();

		if (*SubString == 0 || DestLength < SubLength + 1) return -1;

		const TChar* Dest = **this + Clamp(SearchIndex, 0, DestLength - SubLength - 1);
		TChar DestChar = *Dest;

		if (IgnoreCase)
		{
			TChar FindFirstChar = Chars::ToUpper(*SubString++);
			do
			{
				DestChar = Chars::ToUpper(DestChar);
				if (DestChar == FindFirstChar && !PlatformChars::Strnicmp(Dest + 1, SubString, SubLength))
				{
					return int32(Dest - **this);
				}

				DestChar = *(--Dest);
				SearchIndex--;

			} while (SearchIndex);
		}
		else
		{
			TChar FindFirstChar = *SubString++;
			do
			{
				if (DestChar == FindFirstChar && !PlatformChars::Strncmp(Dest + 1, SubString, SubLength))
				{
					return int32(Dest - **this);
				}

				DestChar = *(--Dest);
				SearchIndex--;

			} while (SearchIndex);
		}

		return -1;
	}

	//Return -1 if failed to search
	FORCE_INLINE
	int32 FindChar(const TChar& Find)
	{
		return Strings.Find(Find);
	}

	//Return -1 if failed to search
	FORCE_INLINE
	int32 FindBySelector(const std::function<bool(const TChar&)>& Selector)
	{
		return Strings.FindBySelector(Selector);
	}

	//Return the replaced char number
	int32 Replace(const TChar* Search, const TChar* Replacement)
	{
		int32 ReplaceNum = 0;

		if (Len() > 0)
		{

			CHECK( (Search != nullptr) && (Replacement != nullptr) );
			if (PlatformChars::Strcmp(Search, Replacement) == 0) return 0;

			//Not ignore case
			TChar* Address = (TChar*)PlatformChars::Strstr(Strings.Begin(), Search);
			if (Address == nullptr) return 0;

			const int32 SearchLen = (int32)PlatformChars::Strlen(Search);
			const int32 ReplacementLen = (int32)PlatformChars::Strlen(Search);

			if (SearchLen == ReplacementLen)
			{
				while (Address != nullptr)
				{
					ReplaceNum++;

					//Replace
					for (int32 i = 0; i < SearchLen; i++)
					{
						Address[i] = Replacement[i];
					}

					if (Address + SearchLen - Strings.Begin() < Len())
						Address = (TChar*)PlatformChars::Strstr(Address + SearchLen, Search);
					else
						break;
				}
			}
			else
			{
				//Make a copy and rebuild the string during replacement to advoid making the code too messy
				String Copy(*this);
				Clear();

				TChar* CopyAdr = *Copy;
				TChar* ReplaceAdr = *Copy + (Address - Strings.Begin());

				while (ReplaceAdr != nullptr)
				{
					ReplaceNum++;

					*ReplaceAdr = 0;  
					(*this) += CopyAdr;//Added 0 behind, so += will stop at ReplaceAdr
					(*this) += Replacement; //Replace

					*ReplaceAdr = *Search;

					CopyAdr = ReplaceAdr + SearchLen;
					ReplaceAdr = (TChar*)PlatformChars::Strstr(CopyAdr, Search);

				}

				(*this) += CopyAdr;
				
			}

		}

		return ReplaceNum;
	}


	void ToUpper()
	{
		const int32 Length = Len();
		TChar* Data = Strings.Begin();
		for (int32 i = 0; i < Length; i++)
			Data[i] = Chars::ToUpper(Data[i]);
	}

	void ToLower()
	{
		const int32 Length = Len();
		TChar* Data = Strings.Begin();
		for (int32 i = 0; i < Length; i++)
			Data[i] = Chars::ToLower(Data[i]);
	}

	void RemoveSpaces()
	{
		const int32 Length = Len();
		if (Length)
		{
			int32 CopyTo = 0;
			TChar* Data = Strings.Begin();
			for (int32 CopyFrom = 0; CopyFrom < Length; CopyFrom++)
			{
				if (Data[CopyFrom] != ' ')
				{
					Data[CopyTo++] = Data[CopyFrom];
				}
			}

		    Data[CopyTo] = 0;
		}
	}

	bool IsNumeric()
	{
		if (!Len()) return false;

		const TChar* Data = Strings.Begin();
		if (*Data == '-' || *Data == '+') Data++;

		bool HasDot = false;
		while (*Data)
		{
			if (*Data == '.')
			{
				if (HasDot) return false;
				HasDot = true;
			}
			else if(!Chars::IsDigit(*Data))
			{
				return false;
			}

			Data++;
		}

		return true;
	}

	template<typename... Types>
	FORCE_INLINE
	static String Sprintf(const TChar* Format, Types... Args)
	{
		return SprintfInternal(Format, Args...);
	}

	template<typename... Types>
	FORCE_INLINE
	static int32 Snprintf(TChar* Dest, int32 DestSize, const TChar* Format, Types... Args)
	{
		return SnprintfInternal(Dest, DestSize, Format, Args...);
	}

	FORCE_INLINE static
	String FromInt(int32 Number)
	{
		String Result;
		Result.AppendInt(Number);
		return Result;
	}

	FORCE_INLINE static
	String FromFloat(double Number)
	{
		String Result;
		Result.AppendFloat(Number);
		return Result;
	}

	FORCE_INLINE static
	String FromBool(bool Value)
	{
		return Value ? TEXTS("true") : TEXTS("false");
	}


	FORCE_INLINE
	int32 ToInt32()
	{
		return PlatformChars::Atoi(**this);
	}

	FORCE_INLINE
	double ToFloat()
	{
		return PlatformChars::Atof(**this);
	}


	bool ToBool()
	{
		if (
			PlatformChars::Stricmp(**this, TEXTS("true")) == 0 ||
			PlatformChars::Stricmp(**this, TEXTS("yes")) == 0 ||
			PlatformChars::Stricmp(**this, TEXTS("on")) == 0
		   )
		{
			return true;
		}
		else if (
			PlatformChars::Stricmp(**this, TEXTS("false")) == 0 ||
			PlatformChars::Stricmp(**this, TEXTS("no")) == 0 ||
			PlatformChars::Stricmp(**this, TEXTS("off")) == 0
			)
		{
			return false;
		}

		return false;
	}


	template <typename CharType>
	FORCE_INLINE friend
	typename FuncTrigger<IsCharType<CharType>::Value, String>::Type
	operator+(const String& Left, CharType Right)
	{
		//check
		String Value(Left);
		Value.Append(Right);

		return Value;
	}


	template <typename CharType>
	FORCE_INLINE friend
	typename FuncTrigger<IsCharType<CharType>::Value, String>::Type
	operator+(String&& Left, CharType Right)
	{
		//check
		String Value(std::move(Left));
		Value.Append(Right);

		return Value;
	}


	FORCE_INLINE friend
	String operator+(const String& Left, const String& Right)
	{
		return Concat<const String&, const String&>(Left, Right);
	}

	FORCE_INLINE friend
	String operator+(String&& Left, const String& Right)
	{
		return Concat<String&&, const String&>(std::move(Left), Right);
	}

	FORCE_INLINE friend
	String operator+(const String& Left, String&& Right)
	{
		return Concat<const String&, String&&>(Left, std::move(Right));
	}

	FORCE_INLINE friend
	String operator+(String&& Left, String&& Right)
	{
		return Concat<String&&, String&&>(std::move(Left), std::move(Right));
	}

	FORCE_INLINE friend
	String operator+(const String& Left, const TChar* Right)
	{
		return ConcatStringAndTChar<const String&>(Left, Right);
	}

	FORCE_INLINE friend
	String operator+(String&& Left, const TChar* Right)
	{
		return ConcatStringAndTChar<String&&>(std::move(Left), Right);
	}

	FORCE_INLINE friend
	String operator+(const TChar* Left, const String& Right)
	{
		return ConcatTCharAndString<const String&>(Left, Right);
	}

	FORCE_INLINE friend
	String operator+(const TChar* Left, String&& Right)
	{
		return ConcatTCharAndString<String&&>(Left, std::move(Right));
	}

	FORCE_INLINE friend
	bool operator==(const String& Left, const String& Right)
	{
		return PlatformChars::Stricmp(*Left, *Right) == 0;
	}

	FORCE_INLINE friend
	bool operator!=(const String& Left, const String& Right)
	{
		return PlatformChars::Stricmp(*Left, *Right) != 0;
	}




protected:
	template <typename LeftType, typename RightType>
	FORCE_INLINE static
	String Concat(typename ExplicitType<LeftType>::Type Left, typename ExplicitType<RightType>::Type Right)
	{
		//check
		if (Left.IsClear())
		{
			return std::move(Right);
		}

		String Value(std::move(Left));
		Value.Append(Right.Strings.Begin(), Right.Len());

		return Value;
	}

	template <typename LeftType>
	FORCE_INLINE static
	String ConcatStringAndTChar(typename ExplicitType<LeftType>::Type Left, const TChar* Right)
	{
		//check
		if (!Right || !*Right)
		{
			return std::move(Left);
		}

		String Value(std::move(Left));
		Value.Append(Right, (int32)PlatformChars::Strlen(Right));

		return Value;
	}

	template <typename RightType>
	FORCE_INLINE static
	String ConcatTCharAndString(const TChar* Left, typename ExplicitType<RightType>::Type Right)
	{
		//check
		if (!Left || !*Left)
		{
			return std::move(Right);
		}

		String Value(std::move(Right));
		Value.Append(Left, (int32)PlatformChars::Strlen(Left));

		return Value;
	}

	
	static String SprintfInternal(const TChar* Format, ...)
	{
		TChar  PrintBuffer[PRINTF_BUFFER_INIT_SIZE];
		TChar* BufferPtr = PrintBuffer;
		int32  BufferSize = PRINTF_BUFFER_INIT_SIZE;
		int32  WroteCount = -1;

		GET_FORMAT_VARARGS(BufferPtr, BufferSize, BufferSize - 1, Format, Format, WroteCount);

		if (WroteCount == -1)
		{
			BufferPtr = nullptr;
			while (WroteCount == -1)
			{
				BufferSize *= 2;
				BufferPtr = (TChar*)Memory::Realloc(BufferPtr, BufferSize * sizeof(TChar));
				GET_FORMAT_VARARGS(BufferPtr, BufferSize, BufferSize - 1, Format, Format, WroteCount);
			}
		}

		BufferPtr[WroteCount] = 0;
		
		String Result(BufferPtr);
		if (BufferSize != PRINTF_BUFFER_INIT_SIZE)
			Memory::Free(BufferPtr);

		return Result;
	}

	static int32 SnprintfInternal(TChar* Dest, int32 DestSize, const TChar* Format, ...)
	{
		int32  WroteCount = -1;

		GET_FORMAT_VARARGS(Dest, DestSize, DestSize - 1, Format, Format, WroteCount);

		if (WroteCount != -1)
		{
			Dest[WroteCount] = 0;
		}

		return WroteCount;
	}
protected:
	Array<TChar> Strings;


};



