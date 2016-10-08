#include "stdafx.h"
#include "AxUI.h"
#include "AxControl.h"
#include "Module.h"

#define TIMER_CHECK_MAINUI	0x20	// 检测主进程是否存在
#define TIMER_SOUND_BLOCK	0x21

HWND CAxUI::sm_hAxUIWnd = NULL;

static DWORD WINAPI AxUIProc(LPVOID lParam)
{
	CAxControl::GlobalInit();

	CAxUI ui;
	ui.Create(HWND_MESSAGE);

	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
		::DispatchMessage(&msg);

	CAxUI::sm_hAxUIWnd = NULL;
	return 0;
}

void CAxUI::InitInMainProcess()
{
	CloseHandle((HANDLE)::CreateThread(NULL, 0, AxUIProc, NULL, 0, NULL));
}

CAxUI::CAxUI() : m_bCookieInited(false), m_bBlockSoundOK(false)
{
}

CAxUI::~CAxUI()
{
	CAxUI::sm_hAxUIWnd = NULL;
}

LRESULT CAxUI::OnCreate(UINT, WPARAM, LPARAM, BOOL&)
{
	CAxUI::sm_hAxUIWnd = m_hWnd;
	::PostMessage(Common::Module::GetMainUIUtil(), WM_MAINUIUTIL_NOTIFY_AXUI_CREATED, (WPARAM)m_hWnd, 0);
	return 0;
}

LRESULT CAxUI::OnCreateNewPage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hChildFrame = (HWND)wParam;
	m_setChildFrms.insert(hChildFrame);
	CloseHandle((HANDLE)::CreateThread(NULL, 0, CAxControl::AxControlPageProc, hChildFrame, 0, NULL));
	return 0;
}

LRESULT CAxUI::OnResponseHeader(UINT, WPARAM wparam, LPARAM lparam, BOOL&)
{
	return 0;
}
