#pragma once


#include "HAL/Dialog/DialogEnum.h"
#include "Global/GlobalType.h"


class WindowsDialog
{
public:
	static DialogReturnType Open(DialogType OpenType, const TChar* Title, const TChar* Message);

};

typedef WindowsDialog PlatformDialog;