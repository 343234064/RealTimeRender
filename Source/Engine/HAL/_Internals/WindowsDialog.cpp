#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "HAL/Platforms/Windows/WindowsDialog.h"



DialogReturnType WindowsDialog::Open(DialogType OpenType, const TChar* Title, const TChar* Message)
{
	HWND Parent = NULL;

	switch (OpenType)
	{
	case DialogType::Ok:
	{
		MessageBox(Parent, Message, Title, MB_OK | MB_SYSTEMMODAL);
		return DialogReturnType::Ok;
	}
	case DialogType::OkCancel:
	{
		int32 Result = MessageBox(Parent, Message, Title, MB_OKCANCEL | MB_SYSTEMMODAL);
		return (Result == IDOK) ? DialogReturnType::Ok : DialogReturnType::Cancel;
	}
	case DialogType::YesNo:
	{
		int32 Result = MessageBox(Parent, Message, Title, MB_YESNO | MB_SYSTEMMODAL);
		return (Result == IDYES) ? DialogReturnType::Yes : DialogReturnType::No;
	}
	case DialogType::RetryCancel:
	{
		int32 Result = MessageBox(Parent, Message, Title, MB_RETRYCANCEL | MB_SYSTEMMODAL);
		return (Result == IDRETRY) ? DialogReturnType::Retry : DialogReturnType::Cancel;
	}
	case DialogType::RetryContinueCancel:
	{
		int32 Result = MessageBox(Parent, Message, Title, MB_CANCELTRYCONTINUE | MB_SYSTEMMODAL);
		return (Result == IDCANCEL) ? DialogReturnType::Cancel : ((Result == IDCONTINUE) ? DialogReturnType::Continue : DialogReturnType::Retry);
	}
	default:
		break;
	}

	return DialogReturnType::Cancel;
}

DialogReturnType WindowsDialog::Open(DialogType OpenType, const ANSICHAR* Title, const ANSICHAR* Message)
{
	HWND Parent = NULL;

	switch (OpenType)
	{
	case DialogType::Ok:
	{
		MessageBoxA(Parent, Message, Title, MB_OK | MB_SYSTEMMODAL);
		return DialogReturnType::Ok;
	}
	case DialogType::OkCancel:
	{
		int32 Result = MessageBoxA(Parent, Message, Title, MB_OKCANCEL | MB_SYSTEMMODAL);
		return (Result == IDOK) ? DialogReturnType::Ok : DialogReturnType::Cancel;
	}
	case DialogType::YesNo:
	{
		int32 Result = MessageBoxA(Parent, Message, Title, MB_YESNO | MB_SYSTEMMODAL);
		return (Result == IDYES) ? DialogReturnType::Yes : DialogReturnType::No;
	}
	case DialogType::RetryCancel:
	{
		int32 Result = MessageBoxA(Parent, Message, Title, MB_RETRYCANCEL | MB_SYSTEMMODAL);
		return (Result == IDRETRY) ? DialogReturnType::Retry : DialogReturnType::Cancel;
	}
	case DialogType::RetryContinueCancel:
	{
		int32 Result = MessageBoxA(Parent, Message, Title, MB_CANCELTRYCONTINUE | MB_SYSTEMMODAL);
		return (Result == IDCANCEL) ? DialogReturnType::Cancel : ((Result == IDCONTINUE) ? DialogReturnType::Continue : DialogReturnType::Retry);
	}
	default:
		break;
	}

	return DialogReturnType::Cancel;
}
