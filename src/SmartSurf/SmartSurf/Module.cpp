#include "stdafx.h"
#include "Module.h"

HWND g_hWndMainUIUtil = NULL;
HWND Common::Module::GetMainUIUtil()
{
	return g_hWndMainUIUtil;
}

void Common::Module::SetMainUIUtil(HWND hWnd)
{
	g_hWndMainUIUtil = hWnd;
}
