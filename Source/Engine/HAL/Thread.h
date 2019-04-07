/************************************
Thread relatives


*************************************/
#pragma once

#include "Global/GlobalType.h"
#include "HAL/Platform.h"


struct PlatformAffinity
{
	static const uint64 GetMainThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const uint64 GetRenderThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

};