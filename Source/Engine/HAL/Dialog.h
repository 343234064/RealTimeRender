/************************************
System dialog


*************************************/
#pragma once

#include "Global/GlobalConfigs.h"



#if PLATFORM_WINDOWS
#include "HAL/Platforms/Windows/WindowsDialog.h"
#elif PLATFORM_LINUX
#include "HAL/Platforms/Linux/LinuxDialog.h"
#elif PLATFORM_MAC
#include "HAL/Platforms/Mac/MacDialog.h"
#else
#error Unknown platform
#endif
