#include "stdafx.h"
#include "SmartSurf.h"


CComModule g_Module;

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:

	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDCLOSE, OnClose)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow();
		return 0;
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		EndDialog(IDOK);
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		MessageBox(L"别关啊，哥很好的", L"别关啊，哥很好的");
		return 0;
	}

};

class CMyWindow : public CWindowImpl<CMyWindow, CWindow, CFrameWinTraits>
{
public:

	CMyWindow()
	{
		m_hBrush = CreateSolidBrush(RGB(255, 0, 0));
	}
	~CMyWindow()
	{
		DeleteObject(m_hBrush);
	}

	DECLARE_WND_CLASS(_T("Sogou-mini"))

	BEGIN_MSG_MAP(CMyWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestory)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBK)
		COMMAND_ID_HANDLER(IDM_ABOUT, OnAbout)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HMENU hMenu = LoadMenu(g_Module.GetResourceInstance(),  MAKEINTRESOURCE(IDC_SMARTSURF));
		SetMenu(hMenu);
		return 0;
	}
	
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DestroyWindow();
		return 0;
	}

	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PostQuitMessage(0);
		return 0;
	}
	
	LRESULT OnEraseBK(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HDC dc = (HDC)wParam;
		RECT rc;
		this->GetClientRect(&rc);
		FillRect(dc, &rc, m_hBrush);
		return 0;
	}

	LRESULT OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return true;
	}
private:

	HBRUSH		m_hBrush;
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_Module.Init(NULL, hInstance);

	CMyWindow myWindow;
	if (!myWindow.Create(NULL, CWindow::rcDefault, L"Miao-SmartSurf"))
		return 1;
	
	myWindow.ShowWindow(nCmdShow);
	myWindow.UpdateWindow();

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	g_Module.Term();

	return (int) msg.wParam;
}