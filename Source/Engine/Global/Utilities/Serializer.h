/****************************************
Base Serializer class for serialization


*****************************************/
#pragma once

#include "Global/GlobalType.h"
#include "Global/Utilities/String.h"
#include "HAL/Platform.h"



class Serializer
{
public:
	Serializer() {}
	virtual ~Serializer() {}

	Serializer(const Serializer& Other) {}
	Serializer& operator=(const Serializer& Other) {}


	//Needed to be implement by concrete class
	virtual void Serialize(void* Data, int64 Length) {} //In bytes
	
	FORCE_INLINE
	virtual void SerializeByteOrder(void* Data, uint32 Length)
	{
		//WARNING : Byte swaping may needed as the byte order may be inconsistent
		//when sending the Serializer to other machine

		Serialize(Data, Length);

	}


	virtual int64 Size() { return 0; }
	virtual int64 Pos() { return -1; }
	virtual bool  End() { return Pos() >= Size(); }
	virtual void  Seek(int64 Pos) {}
	virtual void  Close() {}
	virtual void  Flush() {}

	virtual String GetSerializerName() const
	{
		return TEXTS("Serializer");
	}

	FORCE_INLINE friend 
	Serializer& operator<<(Serializer& In, ANSICHAR& Data)
	{
		In.Serialize(&Data, 1);
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, WIDECHAR& Data)
	{
		In.SerializeByteOrder(&Data, sizeof(Data));
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, uint8& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, int8& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, uint16& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, int16& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}

	
	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, uint32& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, int32& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, uint64& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, int64& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, float& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, double& Data)
	{
		In.SerializeByteOrder(&Data, 1);
		return In;
	}


	template <typename EnumType>
	FORCE_INLINE friend
    typename FuncTrigger< IsEnumClassType<EnumType>::Value, Serializer& > ::Type
	operator<<(Serializer& In, EnumType& Data)
	{
		return In << static_cast<typename std::underlying_type<EnumType>::type &> (Data);
	}


	FORCE_INLINE friend
	Serializer& operator<<(Serializer& In, bool& Data)
	{
		uint8 D = Data ? (uint8)1 : (uint8)0;
		In.Serialize(&D, 1);
		return In;
	}


	
	friend 
	Serializer& operator<<(Serializer& In, String& Data)
	{
		int32 CharNum = Data.Len();
		if (CharNum)
		{
			CharNum += 1;// char '\0'
			int32 ByteCount = Data.SizePerElement() * CharNum;

			In << CharNum;

			//WARNING : Byte swaping may needed as the byte order may be inconsistent
		    //when sending the Serializer to other machine

			In.Serialize(*Data, ByteCount);
		}
	}
};