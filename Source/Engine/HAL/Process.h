/************************************
Process relatives


*************************************/
#pragma once

#include "Global/GlobalConfigs.h"

#if PLATFORM_WINDOWS
#include "HAL/Platforms/Windows/WindowsProcess.h"
#elif PLATFORM_LINUX
#include "HAL/Platforms/Linux/LinuxProcess.h"
#elif PLATFORM_MAC
#include "HAL/Platforms/Mac/MacProcess.h"
#else
#error Unknown platform
#endif