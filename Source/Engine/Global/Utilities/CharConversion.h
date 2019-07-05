#pragma once

#include "Global/Utilities/Assertion.h"




class UTF16ToUTF8
{
#define UNKNOW_CODEPOINT '?'

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

#define HIGHSURROGATE_START  0xd800
#define HIGHSURROGATE_END    0xdbff
#define LOWSURROGATE_START   0xdc00
#define LOWSURROGATE_END     0xdfff

#define SURROGATE_RANGE_START 0x10000
#define SURROGATE_RANGE_END   0x10FFFF


public:
	static FORCE_INLINE void Convert(ANSICHAR* Dest, int32 DestLen, const TChar* Src, int32 SrcLen)
	{
		//static_assert(sizeof(TChar) == sizeof(uint16), "TChar must be 16bits");
		ConvertInternal(Dest, DestLen, Src, SrcLen);
	}

	static FORCE_INLINE int32 Length(const TChar* Src, int32 SrcLen)
	{
		//static_assert(sizeof(TChar) == sizeof(uint16), "TChar must be 16bits");
		ForCounting Counter;
		ConvertInternal(Counter, SrcLen * 4, Src, SrcLen);
		
		return Counter.GetCount();
	}



private:
	static FORCE_INLINE bool IsHighSurrogate(const uint32 Codepoint)
	{
		return Codepoint >= HIGHSURROGATE_START && Codepoint <= HIGHSURROGATE_END;
	}

	static FORCE_INLINE bool IsLowSurrogate(const uint32 Codepoint)
	{
		return Codepoint >= LOWSURROGATE_START && Codepoint <= LOWSURROGATE_END;
	}

	static FORCE_INLINE bool IsEncodedSurrogate(const uint32 Codepoint)
	{
		return Codepoint >= SURROGATE_RANGE_START && Codepoint <= SURROGATE_RANGE_END;
	}

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
	static void ConvertInternal(DestType& Dest, int32 DestLen, const TChar* Src, const int32 SrcLen)
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




struct TCharToUTF8
{
	static FORCE_INLINE void Convert(ANSICHAR* Dest, int32 DestLen, const TChar* Src, int32 SrcLen)
	{

#if PLATFORM_WINDOWS
		//wchar_t is encoded in utf-16 on windows platform
		UTF16ToUTF8::Convert(Dest, DestLen, Src, SrcLen);
#elif PLATFORM_LINUX
#error Platform code needed
#elif PLATFORM_MAC
#error Platform code needed
#else
#error Unknown platform
#endif
		
	}

	static FORCE_INLINE int32 Length(const TChar* Src, int32 SrcLen)
	{
#if PLATFORM_WINDOWS
		//wchar_t is encoded in utf-16 on windows platform
		return UTF16ToUTF8::Length(Src, SrcLen);
#elif PLATFORM_LINUX
#error Platform code needed
#elif PLATFORM_MAC
#error Platform code needed
#else
#error Unknown platform
#endif

	}
};