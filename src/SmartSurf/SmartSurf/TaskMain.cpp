#include "stdafx.h"
#include "IE/AxUI.h"

int TaskMain()
{
	CAxUI ui;
	ui.Create(HWND_MESSAGE);

	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return 0;
}