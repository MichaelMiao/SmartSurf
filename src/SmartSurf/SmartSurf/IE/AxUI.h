#pragma once
#include "IEDef.h"


class CAxUI : public CWindowImpl<CAxUI>
{

public:
	static void InitInMainProcess();
	static HWND sm_hAxUIWnd;

	CAxUI();
	~CAxUI();

	BEGIN_MSG_MAP_EX(CAxUI)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_AXUI_CREATE_NEWPAGE, OnCreateNewPage)
	END_MSG_MAP()

	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnCreateNewPage(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnResponseHeader(UINT, WPARAM, LPARAM, BOOL&);

private:
	
	bool m_bCookieInited;
	bool m_bBlockSoundOK;
	std::set<HWND> m_setDelayedChildFrms;
	std::set<HWND> m_setChildFrms;
};
