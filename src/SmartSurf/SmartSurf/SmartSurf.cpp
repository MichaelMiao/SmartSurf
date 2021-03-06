﻿#include "stdafx.h"
#include "SmartSurf.h"
#include "TaskMain.h"
#include <shellapi.h>
#include "base/strings/stringbuf.h"


CComModule g_Module;

void RunCmd()
{
	int nArgs;
	LPWSTR *szArglist = ::CommandLineToArgvW(GetCommandLineW(), &nArgs);
	// 启动浏览器内核进程
	if (nArgs == 2 && szArglist[1][0] == '-' && szArglist[1][1] == '\0')
	{
		TaskMain();
	}
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_Module.Init(NULL, hInstance);
	RunCmd();
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	g_Module.Term();

	return (int)msg.wParam;
}