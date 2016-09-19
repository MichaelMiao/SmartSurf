#pragma once
#include "SimpleClientSite.h"
#include "BrowserEvents.h"
#include "IEDef.h"

class CAxControl;
class CWebBrowserEventsManager;
class CBrowserSite;
struct AxThreadStorageStruct
{
	HWND hAxControl;
	HWND hFakeOwner;
	CAxControl *pAxControl;
};

struct AxControlConfig
{
	AxControlConfig()
		: BeHideContextMenu(false)
		, bEscKeyAsQuit(false)
	{
	}
	bool BeHideContextMenu;
	bool bEscKeyAsQuit;
};


class CAxControl : public CWindowImpl<CAxControl>
{
public:
	static void GlobalInit();
	static DWORD WINAPI AxControlPageProc(LPVOID lParam);

public:
	static LRESULT CALLBACK WndProcHookIEThread(int iCode, WPARAM wParam, LPARAM lParam);

	static AxThreadStorageStruct* GetTS();
	static void InitTlsAxWnd();
	static void CreateTls();
	static void DestroyTls();
	static bool IsInMainProcess() { return m_bInMainProcess; }


private:
	static DWORD sm_dwTlsIndex;

public:

	void GetAxControlSize(int *pcx, int *pcy) { *pcx = m_szCurrentAxRect.cx; *pcy = m_szCurrentAxRect.cy; }
private:	// from ax control thread
	CAxControl(HWND hFrame);
	~CAxControl();

	bool PreProcessAccelerator(MSG *msg);
	void OnFinalMessage(_In_ HWND) override;
	void Initialize();

private:
	HRESULT InitWebBrowser();
	HRESULT DoVerbShowWebBrowser();
	bool PreProcessMsg(MSG& msg);
	bool PostProcessMsg(MSG& msg);
	void ShutDown();
	void NavigateToBlank();

	BEGIN_MSG_MAP_EX(CAxControl)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_AX_MOVE_FOCUS_TO_IE, OnMoveFocusToIE)
		MESSAGE_HANDLER(WM_AX_ATTACH_TO_MAINTHREAD, OnAttachToMainThread)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
	END_MSG_MAP()

private:	// for main ui
	
	void OnClose();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnWindowPosChanging(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnWindowPosChanged(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnGetMarshalWebBrowser2CrossThread(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMarshalWebBrowser2CrossProcess(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNetRedirect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRefresh(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnGoBack(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnGoForward(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnStop(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnMoveFocusToIE(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnAttachToMainThread(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDetachToMainThread(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnOleCommand(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnAskFakeOwner(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnShowPupupOwned(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnRegisterExternalCmd(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSetAxConfig(UINT, WPARAM, LPARAM, BOOL&);

private:
	void ReSize(int x, int y, int w, int h);

public:
	bool BeSafeQuit();
	LPCWSTR GetUrl();
	LPCWSTR GetLocationUrl();
	void TryClose();

public:		// for AxControl Members
	const AxControlConfig& GetConfig() const;
	HRESULT GetBrowserObject(IOleObject** ppObject);
	HRESULT GetBrowserObject(IWebBrowser2** ppWebBrowser);
	HRESULT GetHtmlDocument2(IHTMLDocument2** ppOut);
	HWND GetFrameWnd();
	HWND GetFrameWndAndInitialized();
	CWebBrowserEventsManager* GetWebBrowserEventManager();
	CBrowserSite* GetBroserSite() const;

public:

	LPCWSTR GetRefererUrl();
	void SetURLBeforeNav(LPCWSTR szURL);
	bool IsRealNavigated();

private:

	void ShowIEDevTool();

private:
	enum
	{
		NAV_NONE = 0,
		NAV_STARTUP_BLANK,
		NAV_REAL,
		NAV_LEAVE_BLANK,
	};

	AxControlConfig m_Config;
	HWND m_hChildFrame;
	bool m_bInitialized;
	DWORD m_dwRealNavigated;
	bool m_bCloseByUser;
	CComPtr<IWebBrowser2> m_spIE;
	CComPtr<IOleObject> m_spIEObj;
	CComObjPtr<CBrowserSite> m_spBrowserSite;
	CComObjPtr<CWebBrowserEventsManager> m_spBrowserEventManager;

private:
	std::wstring m_strReferer;
	std::wstring m_strUrlBeforeNav;
	static bool m_bInMainProcess;
	SIZE m_szCurrentAxRect;
};
