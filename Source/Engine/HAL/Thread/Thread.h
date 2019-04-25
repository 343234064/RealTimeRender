/************************************
Thread relatives


*************************************/
#pragma once

#include "Global/GlobalConfigs.h"



#if PLATFORM_WINDOWS
#include "HAL/Platforms/Windows/WindowsThread.h"
#elif PLATFORM_LINUX
#include "HAL/Platforms/Linux/LinuxThread.h"
#elif PLATFORM_MAC
#include "HAL/Platforms/Mac/MacThread.h"
#else
#error Unknown platform
#endif
