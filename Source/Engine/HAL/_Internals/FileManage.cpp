#include "HAL/File/FileManage.h"
#include "Log/LogMacros.h"



Serializer* FileManage::CreateFileReader(const TChar* FileName, uint32 Flag, uint32 CachedBufferSize)
{
	FileHandle* Handle = nullptr;
	if (!!(Flag & FileFlag::READASYNC))
	{
		Handle = PlatformFile::OpenReadAsync(FileName, !!(Flag & FileFlag::SHARED_WRITE));
	}
	else
	{
	    Handle = PlatformFile::Open(FileName, AccessType::FOR_READ, !!(Flag & FileFlag::SHARED_WRITE), true, false);
	}
	
	if (Handle == nullptr)
	{
		//log
		return nullptr;
	}
	
	return new FileReadSerializer(Handle, CachedBufferSize);
}



Serializer* FileManage::CreateFileWriter(const TChar* FileName, uint32 Flag, uint32 CachedBufferSize)
{

	FileHandle* Handle = PlatformFile::Open(FileName, AccessType::FOR_WRITE, true, !!(Flag & FileFlag::SHARED_READ), !!(Flag & FileFlag::APPEND));

	if (Handle == nullptr)
	{
		//log
		return nullptr;
	}

	return new FileWriteSerializer(Handle, CachedBufferSize);
}







void FileReadSerializer::Serialize(void* Data, int64 Length)
{
	int64 Pos = HandlePtr->Pos();
	int64 Size = HandlePtr->Size();
	
	if (Pos + Length > Size)
	{
		//log
		return;
	}

	while (Length > 0)
	{
		int64 ReadCount = std::min(Length, CacheDataPos + CacheSize - Pos);
		
		//The data has not pre-cached or read pos is out of the cached area, cache needed
		if (ReadCount <= 0)
		{
			//If the Length is over the total buffer size, pre-cache may will be no sense, so read it to dest now
			if (Length >= BufferSize)
			{
				if (!HandlePtr->Read((uint8*)Data, Length))
				{
					//log
				}
				return;
			}

			//Else do a pre-cache
			if (!PreCache(Pos, Size, Pos))
			{
				//log
				return;
			}
			
			ReadCount = std::min(Length, CacheDataPos + CacheSize - Pos);
			if (ReadCount <= 0)
			{
				//log
				return;
			}
			
		}

		//Read data from cache buffer
		Memory::Copy(Data, BufferPtr + Pos - CacheDataPos, ReadCount);

		Length -= ReadCount;
		Data = (uint8*)Data + ReadCount;
	}


}


bool FileReadSerializer::PreCache(int64& CurrentFilePos, int64& CurrentFileSize, int64& PreCacheStartPos)
{
	if (CurrentFilePos == PreCacheStartPos && (CacheDataPos != CurrentFilePos || !CacheDataPos || !CacheSize))
	{
		CacheDataPos = CurrentFilePos;

		//BufferSize is always the power of 2 (e.g. 4096)£¬ (BufferSize-1) will all be 1 in bit order
		//So (CurrentFilePos & (BufferSize - 1)) result in a mod operation : BufferSize % CurrentFilePos
		CacheSize = std::max(
			std::min(std::min((int64)BufferSize, (int64)(BufferSize - (CurrentFilePos & (BufferSize - 1)))), CurrentFileSize - CurrentFilePos),
			1ll);

		if (!HandlePtr->Read(BufferPtr, CacheSize))
		{
			//log
			return false;
		}
	}

	return true;
}






void FileWriteSerializer::Serialize(void* Data, int64 Length)
{
	//If the Length is over the total buffer size, let it go now
	if (Length >= BufferSize)
	{
		Flush();
		if (!HandlePtr->Write((uint8*)Data, Length))
		{
			GuardedLog(TEXTS("Writing to file failed"));
		}

	}
	else
	{
		int64 FillCount = BufferSize - CacheSize;

		//If the Length is larger than the rest space of the buffer, fill in the cache buffer piece by piece and flush it until the data is small enough to cache in the buffer 
		while (Length > FillCount)
		{
			Memory::Copy(BufferPtr + CacheSize, Data, FillCount);

			CacheSize += FillCount;
			CHECK(CacheSize <= BufferSize && CacheSize >= 0);

			Length -= FillCount;
			Data = (uint8*)Data + FillCount;

			//Write all the cache to file
			Flush();

			FillCount = BufferSize - CacheSize;
		}

		//Cache the data to cache buffer
		if (Length)
		{
			Memory::Copy(BufferPtr + CacheSize, Data, Length);

			CacheSize += Length;
			CHECK(CacheSize <= BufferSize && CacheSize >= 0);
		}

	}

}


void FileWriteSerializer::Flush()
{
	if (CacheSize)
	{
		CHECK(CacheSize <= BufferSize && CacheSize >= 0);
		if (!HandlePtr->Write(BufferPtr, CacheSize))
		{
			GuardedLog(TEXTS("Flushing file failed"));
		}
		CacheSize = 0;
	}
	
	HandlePtr->Flush();
}