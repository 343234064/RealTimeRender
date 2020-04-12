#pragma once


#include "HAL/Dialog/DialogEnum.h"
#include "Global/GlobalType.h"


class WindowsDialog
{
public:
	static DialogReturnType Open(DialogType OpenType, const TChar* Title, const TChar* Message);
	static DialogReturnType Open(DialogType OpenType, const ANSICHAR* Title, const ANSICHAR* Message);

};

typedef WindowsDialog PlatformDialog;