#include "stdafx.h"
#include "ProcessData.h"
#include "IEDef.h"


extern HANDLE g_hFileMappingCmdArgs;

ProcessData::ProcessData() : hProcess(NULL), dwProcessID(0), hAxUI(NULL)
{

}

void ProcessData::CreateNewPageInProcess(HWND hChildFrame)
{
	childFrameSet.insert(hChildFrame);
	PostMessageToProcess(WM_AXUI_CREATE_NEWPAGE, (WPARAM)hChildFrame, NULL);
}

void ProcessData::OnSetAxUIWnd(HWND hAxUIWnd)
{
	hAxUI = hAxUIWnd;
	for (std::list<MSG>::iterator it = msgTempQueue.begin(); it != msgTempQueue.end(); it++)
	{
		::PostMessage(hAxUI, it->message, it->wParam, it->lParam);
	}
	msgTempQueue.clear();
}

bool ProcessData::IsProcessExists() const
{
	if (hAxUI == NULL)
	{
		if (hProcess)
		{
			DWORD dwExitCode = 0;
			::GetExitCodeProcess(hProcess, &dwExitCode);
			if (dwExitCode == STILL_ACTIVE)
				return true;
		}
	}
	else if (::IsWindow(hAxUI))
	{
		// 如果AxUI不是空，但是又不是一个合法的窗口，说明某个ax进程退掉了
		return true;
	}

	return false;
}

void ProcessData::TerminateProcess()
{
	::TerminateProcess(hProcess, 0);
}

void ProcessData::PostMessageToProcess(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (dwProcessID != 0)
	{
		if (hAxUI)
		{
			::PostMessage(hAxUI, uMsg, wParam, lParam);
		}
		else
		{
			MSG msg = { 0, uMsg, wParam, lParam, 0 };
			msgTempQueue.push_back(msg);
		}
	}
}