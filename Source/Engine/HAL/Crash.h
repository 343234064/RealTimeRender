#pragma once

/************************************
Crash handlers relatives


*************************************/
#pragma once

#include "Global/GlobalConfigs.h"


#if PLATFORM_WINDOWS
#include "HAL/Platforms/Windows/WindowsCrash.h"
#elif PLATFORM_LINUX
#include "HAL/Platforms/Linux/LinuxCrash.h"
#elif PLATFORM_MAC
#include "HAL/Platforms/Mac/MacCrash.h"
#else
#error Unknown platform
#endif