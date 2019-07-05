/************************************
Platform Time 



*************************************/
#pragma once

#include "Global/GlobalConfigs.h"



#if PLATFORM_WINDOWS
#include "HAL/Platforms/Windows/WindowsTime.h"
#elif PLATFORM_LINUX
#include "HAL/Platforms/Linux/LinuxTime.h"
#elif PLATFORM_MAC
#include "HAL/Platforms/Mac/MacTime.h"
#else
#error Unknown platform
#endif

