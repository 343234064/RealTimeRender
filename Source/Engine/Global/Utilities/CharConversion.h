#pragma once

#include "Global/Utilities/Assertion.h"

/*
 Ref:https://en.wikipedia.org/wiki/UTF-16 (2 Bytes)
 UTF16: 0x0000 ~ 0xD7FF and 0xE000 ~ 0xFFFF -> Unicode: U+0000 ~ U+D7FF and U+E000 ~ U+FFFF
 UTF16: 0xD800 ~ 0xDFFF(Surrogate pair)     -> Unicode: U+010000 ~ U+10FFFF
 UTF16: None                                -> Unicode: U+D8FF ~ U+DFFF(No characters are encoded in this range)

 Ref:https://en.wikipedia.org/wiki/UTF-8 (1~4 Bytes)
 Unicode: U+0000 ~ U+007F -> UTF8: 0xxxxxxx
 Unicode: U+0080 ~ U+07FF -> UTF8: 110xxxxx 10xxxxxx
 Unicode: U+0800 ~ U+FFFF -> UTF8: 1110xxxx 10xxxxxx 10xxxxxx
 Unicode: U+10000 ~ U+10FFFF -> UTF8: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/

#define UNKNOW_CODEPOINT '?'

#define HIGHSURROGATE_START  0xd800
#define HIGHSURROGATE_END    0xdbff
#define LOWSURROGATE_START   0xdc00
#define LOWSURROGATE_END     0xdfff

#define SURROGATE_RANGE_START 0x10000
#define SURROGATE_RANGE_END   0x10FFFF


struct ForCounting
{
	ForCounting() : Counter(0) {}

	ForCounting& operator* () { return *this; }
	ForCounting& operator++() { ++Counter; return *this; }
	ForCounting& operator++(int) { ++Counter; return *this; }
	ForCounting& operator+=(const int32 Amount) { Counter += Amount; return *this; }
	ForCounting& operator=(const ForCounting& Other) { Counter = Other.Counter; return *this; }
	friend int32 operator-(const ForCounting& Left, const ForCounting& Right) { return Left.Counter - Right.Counter; }

	template <typename T>
	T operator=(T Val) const { return Val; }

	int32 GetCount() const { return Counter; }


private:
	int32 Counter;
};

FORCE_INLINE bool IsHighSurrogate(const uint32 Codepoint)
{
	return Codepoint >= HIGHSURROGATE_START && Codepoint <= HIGHSURROGATE_END;
}

FORCE_INLINE bool IsLowSurrogate(const uint32 Codepoint)
{
	return Codepoint >= LOWSURROGATE_START && Codepoint <= LOWSURROGATE_END;
}

FORCE_INLINE bool IsEncodedSurrogate(const uint32 Codepoint)
{
	return Codepoint >= SURROGATE_RANGE_START && Codepoint <= SURROGATE_RANGE_END;
}

FORCE_INLINE uint32 EncodeSurrogate(const uint16 HighSurrogate, const uint16 LowSurrogate)
{
	return ((HighSurrogate - HIGHSURROGATE_START) << 10) + (LowSurrogate - LOWSURROGATE_START) + 0x10000;
}

FORCE_INLINE void DecodeSurrogate(const uint32 Codepoint, uint16& OutHighSurrogate, uint16& OutLowSurrogate)
{
	const uint32 TmpCodepoint = Codepoint - 0x10000;
	OutHighSurrogate = (TmpCodepoint >> 10) + HIGHSURROGATE_START;
	OutLowSurrogate = (TmpCodepoint & 0x3FF) + LOWSURROGATE_START;
}


class TCharToUTF8Impl
{
public:
	/**
	* Converts the UTF-16 or UTF-32 to UTF-8 string.
	*
	* Dest      The destination buffer of the converted string.
	* DestLen   The length of the destination buffer.
	* Src    The source string to convert.
	* SrcLen The length of the source string.
	*/
	static FORCE_INLINE void Convert16(ANSICHAR* Dest, int32 DestLen, const TChar* Src, int32 SrcLen)
	{
		//static_assert(sizeof(TChar) == sizeof(uint16), "TChar must be 16bits");
		ConvertInternal(Dest, DestLen, Src, SrcLen, false);
	}
	/**
	* Determines the length of the converted string.
	*
	* Src string to read and determine amount of TCHARs to represent
	* SrcLen Length of source string; we will not read past this amount, even if the characters tell us to
	* The length of the string.
	*/
	static FORCE_INLINE int32 Length16(const TChar* Src, int32 SrcLen)
	{
		//static_assert(sizeof(TChar) == sizeof(uint16), "TChar must be 16bits");
		ForCounting Counter;
		ConvertInternal(Counter, SrcLen * 4, Src, SrcLen, false);
		
		return Counter.GetCount();
	}


	static FORCE_INLINE void Convert32(ANSICHAR* Dest, int32 DestLen, const TChar* Src, int32 SrcLen)
	{
		//static_assert(sizeof(TChar) == sizeof(uint16), "TChar must be 16bits");
		ConvertInternal(Dest, DestLen, Src, SrcLen, true);
	}

	static FORCE_INLINE int32 Length32(const TChar* Src, int32 SrcLen)
	{
		//static_assert(sizeof(TChar) == sizeof(uint16), "TChar must be 16bits");
		ForCounting Counter;
		ConvertInternal(Counter, SrcLen * 4, Src, SrcLen, true);

		return Counter.GetCount();
	}

private:

	template <typename DestType>
	static int32 CodepointToUtf8(uint32 UniCodepoint, DestType Dest, uint32 DestRemainByteSize)
	{
		CHECK(DestRemainByteSize >= sizeof(ANSICHAR));

		const DestType DestStart = Dest;

		if (UniCodepoint > SURROGATE_RANGE_END) //Exceed unicode range
		{
			UniCodepoint = UNKNOW_CODEPOINT;
		}
		else if (IsHighSurrogate(UniCodepoint) || IsLowSurrogate(UniCodepoint)) //No characters are encoded in this range
		{
			UniCodepoint = UNKNOW_CODEPOINT;
		}


		if (UniCodepoint < 0x80) // U+0000 ~ U+007F
		{
			*(Dest++) = (ANSICHAR)UniCodepoint;
		}
		else if (UniCodepoint < 0x800) // U+0080 ~ U+07FF
		{
			if (DestRemainByteSize >= 2)
			{
				*(Dest++) = (ANSICHAR)((UniCodepoint >> 6) | 128 | 64); //110 xxxxx
				*(Dest++) = (ANSICHAR)(UniCodepoint & 0x3F) | 128; // 10 xxxxxx
			}
			else
			{
				*(Dest++) = (ANSICHAR)UNKNOW_CODEPOINT;
			}
		}
		else if (UniCodepoint < 0x10000) // U+0800 ~ U+FFFF
		{
			if (DestRemainByteSize >= 3)
			{
				*(Dest++) = (ANSICHAR)((UniCodepoint >> 12) | 128 | 64 | 32); //1110 xxxx
				*(Dest++) = (ANSICHAR)((UniCodepoint >> 6) & 0x3F) | 128; // 10 xxxxxx
				*(Dest++) = (ANSICHAR)(UniCodepoint & 0x3F) | 128; // 10 xxxxxx
			}
			else
			{
				*(Dest++) = (ANSICHAR)UNKNOW_CODEPOINT;
			}
		}
		else // U+10000 ~ U+10FFFF
		{
			if (DestRemainByteSize >= 4)
			{
				*(Dest++) = (ANSICHAR)((UniCodepoint >> 18) | 128 | 64 | 32 | 16); //11110 xxx
				*(Dest++) = (ANSICHAR)((UniCodepoint >> 12) & 0x3F) | 128; // 10 xxxxxx
				*(Dest++) = (ANSICHAR)((UniCodepoint >> 6) & 0x3F) | 128; // 10 xxxxxx
				*(Dest++) = (ANSICHAR)(UniCodepoint & 0x3F) | 128; // 10 xxxxxx
			}
			else
			{
				*(Dest++) = (ANSICHAR)UNKNOW_CODEPOINT;
			}
		}

		return static_cast<int32>(Dest - DestStart);
	}



	template <typename DestType>
	static void ConvertInternal(DestType& Dest, int32 DestLen, const TChar* Src, const int32 SrcLen, bool TCharIs32)
	{
		if (TCharIs32)
		{
			for (int32 i = 0; i < SrcLen; ++i)
			{
				uint32 Codepoint = static_cast<uint32>(Src[i]);

				if (!WriteToBuffer(Codepoint, Dest, DestLen))
				{
					// Could not write data
					return;
				}
			}
		}
		else
		{

			uint32 HighSurrogate = MAX_INT32;
			bool GotHighSurrogate = false;

			for (int32 i = 0; i < SrcLen; i++)
			{
				GotHighSurrogate = HighSurrogate != MAX_INT32;
				uint32 CodePoint = static_cast<uint32>(Src[i]);

				//Check if the codepoint is high surrogate
				if (IsHighSurrogate(CodePoint))
				{
					if (GotHighSurrogate)
					{
						if (!WriteToBuffer(HighSurrogate, Dest, DestLen))
						{
							return;
						}
					}

					HighSurrogate = CodePoint;
					continue;
				}


				if (GotHighSurrogate)
				{
					//If the high surrogate is found, check if next codepoint is low surrogate 
					if (IsLowSurrogate(CodePoint))
					{
						//If so, convert the 2 pair into unicode
						const uint32 LowSurrogate = CodePoint;
						CodePoint = ((HighSurrogate - HIGH_SURROGATE_START) << 10) + (LowSurrogate - LOW_SURROGATE_START) + 0x10000;
					}
					else
					{
						if (!WriteToBuffer(HighSurrogate, Dest, DestLen))
						{
							return;
						}
					}

					HighSurrogate = MAX_INT32;
				}


				//Finally write to buffer
				if (!WriteToBuffer(CodePoint, Dest, DestLen))
				{
					return;
				}
			}
		}
	}


	template <typename DestType>
	static bool WriteToBuffer(const int32 CodePoint, DestType& Dest, int32& DestLen)
	{
		int32 Wrote = CodepointToUtf8(CodePoint, Dest, DestLen);
		if (Wrote < 1) return false;

		Dest += Wrote;
		DestLen -= Wrote;
		return true;
	}
};



class UTF8ToTCharImpl
{
public:
	/**
	 * Converts the UTF-8 string to UTF-16 or UTF-32.
	 *
	 * Dest      The destination buffer of the converted string.
	 * DestLen   The length of the destination buffer.
	 * Src    The source string to convert.
	 * SrcLen The length of the source string.
	 */
	FORCE_INLINE
	static void Convert16(TChar* Dest, const int32 DestLen, const ANSICHAR* Src, const int32 SrcLen)
	{
		ConvertInternal(Dest, DestLen, Src, SrcLen, false);
	}

	/**
	 * Determines the length of the converted string.
	 *
	 * Src string to read and determine amount of TCHARs to represent
	 * SrcLen Length of source string; we will not read past this amount, even if the characters tell us to
	 * The length of the string in UTF-16 or UTF-32 characters.
	 */
	FORCE_INLINE
	static int32 Length16(const ANSICHAR* Src, const int32 SrcLen)
	{
		ForCounting Counter;
		ConvertInternal(Counter, MAX_INT32, Src, SrcLen, false);

		return Counter.GetCount();
	}


	FORCE_INLINE
	static void Convert32(TChar* Dest, const int32 DestLen, const ANSICHAR* Src, const int32 SrcLen)
	{
		ConvertInternal(Dest, DestLen, Src, SrcLen, true);
	}

	FORCE_INLINE
	static int32 Length32(const ANSICHAR* Src, const int32 SrcLen)
	{
		ForCounting Counter;
		ConvertInternal(Counter, MAX_INT32, Src, SrcLen, true);

		return Counter.GetCount();
	}

private:
	static uint32 CodepointFromUtf8(const ANSICHAR*& SourceString, const uint32 SourceLengthRemaining)
	{
		CHECK(SourceLengthRemaining > 0);

		const ANSICHAR* OctetPtr = SourceString;

		uint32 Codepoint = 0;
		uint32 Octet = (uint32)((uint8)*SourceString);
		uint32 Octet2, Octet3, Octet4;

		if (Octet < 128)  // one octet char: 0 to 127
		{
			++SourceString;  // skip to next possible start of codepoint.
			return Octet;
		}
		else if (Octet < 192)  // bad (starts with 10xxxxxx).
		{
			// Apparently each of these is supposed to be flagged as a bogus
			//  char, instead of just resyncing to the next valid codepoint.
			++SourceString;  // skip to next possible start of codepoint.
			return UNKNOW_CODEPOINT;
		}
		else if (Octet < 224)  // two octets
		{
			// Ensure our string has enough characters to read from
			if (SourceLengthRemaining < 2)
			{
				// Skip to end and write out a single char (we always have room for at least 1 char)
				SourceString += SourceLengthRemaining;
				return UNKNOW_CODEPOINT;
			}

			Octet -= (128 + 64);
			Octet2 = (uint32)((uint8) * (++OctetPtr));
			if ((Octet2 & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Codepoint = ((Octet << 6) | (Octet2 - 128));
			if ((Codepoint >= 0x80) && (Codepoint <= 0x7FF))
			{
				SourceString += 2;  // skip to next possible start of codepoint.
				return Codepoint;
			}
		}
		else if (Octet < 240)  // three octets
		{
			// Ensure our string has enough characters to read from
			if (SourceLengthRemaining < 3)
			{
				// Skip to end and write out a single char (we always have room for at least 1 char)
				SourceString += SourceLengthRemaining;
				return UNKNOW_CODEPOINT;
			}

			Octet -= (128 + 64 + 32);
			Octet2 = (uint32)((uint8) * (++OctetPtr));
			if ((Octet2 & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet3 = (uint32)((uint8) * (++OctetPtr));
			if ((Octet3 & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Codepoint = (((Octet << 12)) | ((Octet2 - 128) << 6) | ((Octet3 - 128)));

			// UTF-8 characters cannot be in the UTF-16 surrogates range
			if (IsHighSurrogate(Codepoint) || IsLowSurrogate(Codepoint))
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			// 0xFFFE and 0xFFFF are illegal, too, so we check them at the edge.
			if ((Codepoint >= 0x800) && (Codepoint <= 0xFFFD))
			{
				SourceString += 3;  // skip to next possible start of codepoint.
				return Codepoint;
			}
		}
		else if (Octet < 248)  // four octets
		{
			// Ensure our string has enough characters to read from
			if (SourceLengthRemaining < 4)
			{
				// Skip to end and write out a single char (we always have room for at least 1 char)
				SourceString += SourceLengthRemaining;
				return UNKNOW_CODEPOINT;
			}

			Octet -= (128 + 64 + 32 + 16);
			Octet2 = (uint32)((uint8) * (++OctetPtr));
			if ((Octet2 & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet3 = (uint32)((uint8) * (++OctetPtr));
			if ((Octet3 & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet4 = (uint32)((uint8) * (++OctetPtr));
			if ((Octet4 & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Codepoint = (((Octet << 18)) | ((Octet2 - 128) << 12) |
				((Octet3 - 128) << 6) | ((Octet4 - 128)));
			if ((Codepoint >= 0x10000) && (Codepoint <= 0x10FFFF))
			{
				SourceString += 4;  // skip to next possible start of codepoint.
				return Codepoint;
			}
		}
		// Five and six octet sequences became illegal in rfc3629.
		//  We throw the codepoint away, but parse them to make sure we move
		//  ahead the right number of bytes and don't overflow the buffer.
		else if (Octet < 252)  // five octets
		{
			// Ensure our string has enough characters to read from
			if (SourceLengthRemaining < 5)
			{
				// Skip to end and write out a single char (we always have room for at least 1 char)
				SourceString += SourceLengthRemaining;
				return UNKNOW_CODEPOINT;
			}

			Octet = (uint32)((uint8) * (++OctetPtr));
			if ((Octet & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet = (uint32)((uint8) * (++OctetPtr));
			if ((Octet & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet = (uint32)((uint8) * (++OctetPtr));
			if ((Octet & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet = (uint32)((uint8) * (++OctetPtr));
			if ((Octet & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			SourceString += 5;  // skip to next possible start of codepoint.
			return UNKNOW_CODEPOINT;
		}

		else  // six octets
		{
			// Ensure our string has enough characters to read from
			if (SourceLengthRemaining < 6)
			{
				// Skip to end and write out a single char (we always have room for at least 1 char)
				SourceString += SourceLengthRemaining;
				return UNKNOW_CODEPOINT;
			}

			Octet = (uint32)((uint8) * (++OctetPtr));
			if ((Octet & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet = (uint32)((uint8) * (++OctetPtr));
			if ((Octet & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet = (uint32)((uint8) * (++OctetPtr));
			if ((Octet & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet = (uint32)((uint8) * (++OctetPtr));
			if ((Octet & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			Octet = (uint32)((uint8) * (++OctetPtr));
			if ((Octet & (128 + 64)) != 128)  // Format isn't 10xxxxxx?
			{
				++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
				return UNKNOW_CODEPOINT;
			}

			SourceString += 6;  // skip to next possible start of codepoint.
			return UNKNOW_CODEPOINT;
		}

		++SourceString;  // Sequence was not valid UTF-8. Skip the first byte and continue.
		return UNKNOW_CODEPOINT;  // catch everything else.
	}

	/**
	 * Read Source string, converting the data from UTF-8 into UTF-16, and placing these in the Destination
	 */
	template <typename DestBufferType>
	static void ConvertInternal(DestBufferType& ConvertedBuffer, int32 DestLen, const ANSICHAR* Src, const int32 SrcLen, bool TCharIs32)
	{
		const ANSICHAR* SourceEnd = Src + SrcLen;
		while (Src < SourceEnd && DestLen > 0)
		{
			// Read our codepoint, advancing the source pointer
			uint32 Codepoint = CodepointFromUtf8(Src, SourceEnd - Src);

			if (!TCharIs32)
			{
				// We want to write out two chars
				if (IsEncodedSurrogate(Codepoint))
				{
					// We need two characters to write the surrogate pair
					if (DestLen >= 2)
					{
						uint16 HighSurrogate = 0;
						uint16 LowSurrogate = 0;
						DecodeSurrogate(Codepoint, HighSurrogate, LowSurrogate);

						*(ConvertedBuffer++) = (TChar)HighSurrogate;
						*(ConvertedBuffer++) = (TChar)LowSurrogate;
						DestLen -= 2;
						continue;
					}

					// If we don't have space, write a bogus character instead (we should have space for it)
					Codepoint = UNKNOW_CODEPOINT;
				}
				else if (Codepoint > SURROGATE_RANGE_END)
				{
					// Ignore values higher than the supplementary plane range
					Codepoint = UNKNOW_CODEPOINT;
				}
			}

			* (ConvertedBuffer++) = Codepoint;
			--DestLen;
		}
	}
};

struct TCharToUTF8
{
	static FORCE_INLINE void Convert(Array<ANSICHAR>& OutConvertedUTF8Text, const TChar* Src)
	{
		const int32 DataLength = (int32)PlatformChars::Strlen(Src);
		const int32 ConvertedLength = TCharToUTF8::Length(Src, DataLength);
		OutConvertedUTF8Text.AddUninitialize(ConvertedLength);

#if PLATFORM_WINDOWS
		//wchar_t is encoded in utf-16 on windows platform
		TCharToUTF8Impl::Convert16(OutConvertedUTF8Text.Begin(), ConvertedLength, Src, DataLength);
#elif PLATFORM_LINUX
#error Platform code needed
#elif PLATFORM_MAC
#error Platform code needed
#else
#error Unknown platform
#endif
		
	}

	//Determines the length of the converted string.
	static FORCE_INLINE int32 Length(const TChar* Src, int32 SrcLen)
	{
#if PLATFORM_WINDOWS
		//wchar_t is encoded in utf-16 on windows platform
		return TCharToUTF8Impl::Length16(Src, SrcLen);
#elif PLATFORM_LINUX
#error Platform code needed
#elif PLATFORM_MAC
#error Platform code needed
#else
#error Unknown platform
#endif

	}
};


struct UTF8ToTChar
{
	static FORCE_INLINE void Convert( Array<TChar>& OutConvertedTCharText, const ANSICHAR* Src)
	{
		const int32 DataLength = (int32)PlatformChars::Strlen(Src);
		const int32 ConvertedLength = UTF8ToTChar::Length(Src, DataLength);
		OutConvertedTCharText.AddUninitialize(ConvertedLength);

#if PLATFORM_WINDOWS
		//wchar_t is encoded in utf-16 on windows platform
		UTF8ToTCharImpl::Convert16(OutConvertedTCharText.Begin(), ConvertedLength, Src, DataLength);
#elif PLATFORM_LINUX
#error Platform code needed
#elif PLATFORM_MAC
#error Platform code needed
#else
#error Unknown platform
#endif

	}

	//Determines the length of the converted string.
	static FORCE_INLINE int32 Length(const ANSICHAR* Src, int32 SrcLen)
	{
#if PLATFORM_WINDOWS
		//wchar_t is encoded in utf-16 on windows platform
		return UTF8ToTCharImpl::Length16(Src, SrcLen);
#elif PLATFORM_LINUX
#error Platform code needed
#elif PLATFORM_MAC
#error Platform code needed
#else
#error Unknown platform
#endif

	}
};