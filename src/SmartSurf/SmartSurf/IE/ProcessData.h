// Author: linjian
#pragma once


struct ProcessData
{
	ProcessData();

	HANDLE hProcess;
	DWORD dwProcessID;
	HWND hAxUI;

	std::list<MSG> msgTempQueue;
	std::set<HWND> childFrameSet;

	void CreateNewPageInProcess(HWND hChildFrame);
	void OnSetAxUIWnd(HWND hAxUIWnd);
	
	bool IsProcessExists() const;
	void TerminateProcess();
	void PostMessageToProcess(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
