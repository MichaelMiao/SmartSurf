#include "stdafx.h"
#include "MainUIUtil.h"
#include "Module.h"
#include "IE\AxUI.h"


static DWORD WINAPI MainUIUtilProc(LPVOID lParam)
{
	CoInitialize(NULL);
	CMainUIUtil *pMainUIUtil = (CMainUIUtil*)lParam;
	pMainUIUtil->Create(HWND_MESSAGE);
	CAxUI::InitInMainProcess();

	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	CoUninitialize();
	return 0;
}

CMainUIUtil* CMainUIUtil::Inst()
{
	static CMainUIUtil _s;
	return &_s;
}

CMainUIUtil::CMainUIUtil() : m_hAttachWnd(NULL)
{

}

void CMainUIUtil::Init(HWND hWnd)
{
	m_hAttachWnd = hWnd;
	::CloseHandle(::CreateThread(NULL, 0, MainUIUtilProc, this, 0, NULL));
}

int CMainUIUtil::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_processMgr.Init();
	::PostMessage(m_hWnd, WM_MAINUITIL_CREATEPAGE, (WPARAM)m_hAttachWnd, 0);
	Common::Module::SetMainUIUtil(m_hWnd);
	return 0;
}
