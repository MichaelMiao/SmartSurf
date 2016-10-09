#pragma once
#include "ProcessData.h"
#include "IEDef.h"

class CProcessManager
{
public:


	CProcessManager();
	~CProcessManager();

	void Init();


private:

	void CreateProcess();


	BEGIN_MSG_MAP_EX(CProcessManager)
		MESSAGE_HANDLER(WM_MAINUITIL_CREATEPAGE, OnCreatePage)
		MESSAGE_HANDLER(WM_MAINUIUTIL_NOTIFY_AXUI_CREATED, OnAxUICreate)
	END_MSG_MAP()

	LRESULT OnCreatePage(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnAxUICreate(UINT, WPARAM, LPARAM, BOOL&);

private:
};
