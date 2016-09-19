#include "stdafx.h"
#include "AxControl.h"
#include "BrowserEvents.h"
#include "SimpleClientSite.h"
#include "IEDef.h"

DWORD CAxControl::sm_dwTlsIndex = 0;
bool CAxControl::m_bInMainProcess = false;

void CAxControl::InitTlsAxWnd()
{
	sm_dwTlsIndex = ::TlsAlloc();
	TuoAssert(TLS_OUT_OF_INDEXES != sm_dwTlsIndex);
}

void CAxControl::CreateTls()
{
	AxThreadStorageStruct *pTS = new AxThreadStorageStruct;
	memset(pTS, 0, sizeof(AxThreadStorageStruct));
	::TlsSetValue(sm_dwTlsIndex, pTS);
}

AxThreadStorageStruct* CAxControl::GetTS()
{
	return (AxThreadStorageStruct*)::TlsGetValue(sm_dwTlsIndex);
}

void CAxControl::GlobalInit()
{
	::CoInitialize(NULL);

	CAxControl::InitTlsAxWnd();
}

DWORD WINAPI CAxControl::AxControlPageProc(LPVOID lParam)
{
	::OleInitialize(NULL);

	CAxControl::CreateTls();
	HHOOK hAxHook = ::SetWindowsHookEx(WH_CALLWNDPROC, CAxControl::WndProcHookIEThread, 0, ::GetCurrentThreadId());

	// 【linjian】以后做防假死用
	class CFakeOwner : public CWindowImpl<CFakeOwner>
	{
	public:

		CFakeOwner() {}
		~CFakeOwner() { DestroyWindow(); }

		BEGIN_MSG_MAP_EX(CFakeOwner)
		END_MSG_MAP()
		DECLARE_WND_CLASS(L"Aurora_Ax_FakeOwner")
	};

	CFakeOwner fakeOwner;
	fakeOwner.Create(NULL, CRect(0, 0, 0, 0), NULL, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);

	HWND hChildFrame = (HWND)lParam;
	CAxControl* pNewAx = new CAxControl(hChildFrame);
	if (GetTS())
		GetTS()->pAxControl = pNewAx;

	CRect rcClient;
	::GetClientRect(hChildFrame, &rcClient);
	HWND hAxWnd = pNewAx->Create(fakeOwner, rcClient, NULL, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);

	if (::IsWindow(hChildFrame))
	{
		// 通知对应的childframe，axcontrol已经创建好
		pNewAx->SetWindowLongPtr(GWLP_USERDATA, (LONG_PTR)hChildFrame);
		//pNewAx->ModifyStyle(WS_POPUP, WS_CHILD);
		//pNewAx->SetParent(hChildFrame);

		if (GetTS())
		{
			GetTS()->hAxControl = pNewAx->m_hWnd;
			GetTS()->hFakeOwner = fakeOwner.m_hWnd;
		}
		pNewAx->Initialize();

		CRect rcChildFrame;
		::GetWindowRect(hChildFrame, &rcChildFrame);
		::SetWindowPos(pNewAx->m_hWnd, ::GetAncestor(hChildFrame, GA_ROOT), rcChildFrame.left, rcChildFrame.top, rcChildFrame.Width(), rcChildFrame.Height(), SWP_NOACTIVATE);

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (pNewAx->PreProcessMsg(msg))
				continue;
			// 交给内核处理键盘消息
			if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
			{
				if (pNewAx->PreProcessAccelerator(&msg))
					continue;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else
	{
		::DestroyWindow(hAxWnd);
	}
	delete pNewAx;

	::UnhookWindowsHookEx(hAxHook);
	::OleUninitialize();
	return 0;
}

CAxControl::CAxControl(HWND hFrame)
	: m_hChildFrame(hFrame), m_bInitialized(false),
	m_bCloseByUser(false), m_dwRealNavigated(NAV_NONE)
{
	static bool bInit = false;
	if (!bInit)
	{
		bInit = true;
		DWORD dwPid = 0;
		GetWindowThreadProcessId(m_hChildFrame, &dwPid);
		m_bInMainProcess = GetCurrentProcessId() == dwPid;
	}
}

CAxControl::~CAxControl()
{
	ShutDown();
}

bool CAxControl::PreProcessAccelerator(MSG *msg)
{
	if (m_spIE)
	{
		CComQIPtr<IOleInPlaceActiveObject, &__uuidof(IOleInPlaceActiveObject)> spInPlaceActiveObject(m_spIE);
		if (spInPlaceActiveObject && spInPlaceActiveObject->TranslateAccelerator(msg) == S_OK)
			return true;
	}
	return false;
}


void CAxControl::OnFinalMessage(_In_ HWND)
{
	PostQuitMessage(0);
}

HRESULT CAxControl::InitWebBrowser()
{
	TuoAssert(!m_spBrowserSite);

	m_spBrowserSite = CreateComInstance<CBrowserSite>(this);

	CComPtr<IClassFactory> spClassFactory;
	HRESULT hr = ::CoGetClassObject(CLSID_WebBrowser, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, NULL, IID_IClassFactory, (void**)&spClassFactory);
	if (SUCCEEDED(hr))
		hr = spClassFactory->CreateInstance(NULL, IID_IOleObject, (void**)&m_spIEObj);

	CComQIPtr<IOleClientSite> spClientSite = m_spBrowserSite;
	if (SUCCEEDED(hr))
	{
		hr = m_spIEObj->SetClientSite(spClientSite);
		::OleSetContainedObject(m_spIEObj, TRUE);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_spIEObj->QueryInterface(IID_IWebBrowser2, (void**)&m_spIE);
		CRect rcClient;
		GetClientRect(&rcClient);
		ReSize(0, 0, rcClient.Width(), rcClient.Height());
	}

	if (SUCCEEDED(hr))
	{
		m_spBrowserEventManager = CreateComInstance<CWebBrowserEventsManager>(this);
		m_spBrowserEventManager->Advise(m_spIE);
	}

	if (SUCCEEDED(hr))
		hr = DoVerbShowWebBrowser();

	if (SUCCEEDED(hr))
		m_spIE->put_RegisterAsBrowser(VARIANT_TRUE);

	::PostMessage(m_hWnd, WM_AX_ATTACH_TO_MAINTHREAD, 0, 0);
	return hr;
}

void CAxControl::Initialize()
{
	// 必须要先导航到blank，否则会影响防假死功能
// 	bool bNoNav = ::GetProp(m_hChildFrame, CHILD_FRM_NONAV_PROP) != NULL;
// 	if (!bNoNav)
// 	{
// 		m_dwRealNavigated = NAV_STARTUP_BLANK;
// 		NavigateToBlank();
// 	}
// 	else
// 	{
// 		m_dwRealNavigated = NAV_REAL;
// 	}
	if (m_spIE)
	{
		CComBSTR bstrUrl = L"www.baidu.com";
		HRESULT hr = m_spIE->Navigate(bstrUrl, NULL, NULL, NULL, NULL);
	}
	m_bInitialized = true;
}

HRESULT CAxControl::DoVerbShowWebBrowser()
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	return m_spIEObj->DoVerb(OLEIVERB_SHOW, NULL, m_spBrowserSite, -1, m_hWnd, &rect);
}

void CAxControl::NavigateToBlank()
{
	if (m_spIE)
	{
		CComBSTR bstrUrl = L"about:blank";
		HRESULT hr = m_spIE->Navigate(bstrUrl, NULL, NULL, NULL, NULL);
	}
}

bool CAxControl::PreProcessMsg(MSG& msg)
{
	if (m_Config.bEscKeyAsQuit && msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
	{
		::PostMessage(GetFrameWnd(), WM_CLOSE, 0, 0);
		return true;
	}

	return false;
}

void CAxControl::ShutDown()
{
	if (m_spBrowserEventManager)
	{
		m_spBrowserEventManager->Unadvise();
		m_spBrowserEventManager->SetOwner(NULL);
		m_spBrowserEventManager.Release();
	}

	if (m_spIEObj)
	{
		HRESULT hr = m_spIEObj->Close(OLECLOSE_NOSAVE);
		m_spIEObj.Release();
	}

	if (m_spIE)
		m_spIE.Release();
	if (m_spBrowserSite)
	{
		m_spBrowserSite->SetOwner(NULL);
		m_spBrowserSite.Release();
	}
}

void CAxControl::OnClose()
{
	m_bCloseByUser = true;
	TryClose();
}

bool CAxControl::BeSafeQuit()
{
	return m_bCloseByUser == true;
}

int CAxControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_szCurrentAxRect.cx = lpCreateStruct->cx;
	m_szCurrentAxRect.cy = lpCreateStruct->cy;
	InitWebBrowser();
	::PostMessage(m_hChildFrame, WM_AX_CONTROL_CREATED, (WPARAM)m_hWnd, 0);
	return TRUE;
}

void CAxControl::TryClose()
{
	bool bSafeQuit = BeSafeQuit();
	// check quit condition
	if (bSafeQuit)
	{
		// 先导航到空白页，期望能清理干净
		if (m_spIE && m_spBrowserEventManager)
		{
			m_dwRealNavigated = NAV_LEAVE_BLANK;
			NavigateToBlank();
			m_spBrowserEventManager->OnWndCloseEvent();
		}
		else
		{
			ShutDown();
		}
	}
}

void CAxControl::OnDestroy()
{
	SetMsgHandled(FALSE);
	// 不要在这里ShutDown
}

LRESULT CAxControl::OnWindowPosChanging(UINT, WPARAM, LPARAM lParam, BOOL&)
{
	WINDOWPOS *pWndPos = (WINDOWPOS*)lParam;
	if (pWndPos && (pWndPos->flags & SWP_NOSIZE) == 0)
	{
		m_szCurrentAxRect.cx = pWndPos->cx;
		m_szCurrentAxRect.cy = pWndPos->cy;
	}
	return 0;
}

LRESULT CAxControl::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	bHandled = FALSE;
	WINDOWPOS *pos = (WINDOWPOS*)lParam;
	if ((pos->flags & SWP_NOSIZE) == 0)
	{
		ReSize(0, 0, pos->cx, pos->cy);
		m_szCurrentAxRect.cx = pos->cx;
		m_szCurrentAxRect.cy = pos->cy;
	}
	return 0;
}

void CAxControl::ReSize(int x, int y, int w, int h)
{
	CComQIPtr<IOleInPlaceObject> spInPlaceObj = m_spIEObj;
	if (spInPlaceObj)
	{
		SIZE sz = { w, h };
		SIZE sz2;
		RECT rc = { x, y, x + w, y + h };
		AtlPixelToHiMetric(&sz, &sz2);
		m_spIEObj->SetExtent(DVASPECT_CONTENT, &sz2);
		spInPlaceObj->SetObjectRects(&rc, &rc);
	}
}

LRESULT CAxControl::OnRefresh(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	if (!m_spIE)
		return 0;

	if ((BOOL)wParam)
	{
		VARIANT var;
		::VariantInit(&var);
		var.vt = VT_I4;
		var.lVal = REFRESH_COMPLETELY;
		m_spIE->Refresh2(&var);
		::VariantClear(&var);
	}
	else
	{
		m_spIE->Refresh();
	}
	return 0;
}

LRESULT CAxControl::OnGoBack(UINT, WPARAM, LPARAM, BOOL&)
{
	if (m_spIE)
		m_spIE->GoBack();
	return 0;
}

LRESULT CAxControl::OnGoForward(UINT, WPARAM, LPARAM, BOOL&)
{
	if (m_spIE)
		m_spIE->GoForward();
	return 0;
}

LRESULT CAxControl::OnStop(UINT, WPARAM, LPARAM, BOOL&)
{
	if (m_spIE)
		m_spIE->Stop();
	return 0;
}

LRESULT CAxControl::OnMoveFocusToIE(UINT, WPARAM, LPARAM, BOOL&)
{
	/*
	CComPtr<IHTMLDocument2> spDoc2;
	if (SUCCEEDED(GetHtmlDocument2(&spDoc2)))
	{
	CComPtr<IHTMLElementCollection> spColl;
	if (SUCCEEDED(spDoc2->get_all(&spColl)) && spColl)
	{
	long nCnt = 0;
	spColl->get_length(&nCnt);
	for (long i = 0; i < nCnt; i++)
	{
	CComVariant varIdx = i;
	CComPtr<IDispatch> spTagDisp;
	if (SUCCEEDED(spColl->item(varIdx, varIdx, &spTagDisp)) && spTagDisp)
	{
	short tabindex = 0;
	CComQIPtr<IHTMLElement2> spHtmlElem2(spTagDisp);
	if (SUCCEEDED(spHtmlElem2->get_tabIndex(&tabindex)) && tabindex >= 0)
	{
	spHtmlElem2->focus();
	break;
	}
	}
	}
	}
	}
	*/
	return 0;
}

LRESULT CAxControl::OnAttachToMainThread(UINT uMsg, WPARAM wParam, LPARAM, BOOL&)
{
	if (GetParent() == m_hChildFrame)
		return 0;

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD dwNewStyle = (dwStyle & ~(WS_POPUP | WS_DISABLED)) | WS_CHILD;
	::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);

	::SetParent(m_hWnd, m_hChildFrame);
	AttachThreadInput(GetCurrentThreadId(), GetWindowThreadProcessId(m_hChildFrame, NULL), TRUE);

	RECT rc;
	::GetClientRect(m_hChildFrame, &rc);
	SetWindowPos(NULL, &rc, SWP_SHOWWINDOW | SWP_NOACTIVATE);
	ShowWindow(SW_SHOW);

	HWND hShellEmbedding = ::FindWindowEx(m_hWnd, NULL, L"Shell Embedding", NULL);
	if (hShellEmbedding == NULL)
		return 0;
	HWND hDocObj = ::GetWindow(hShellEmbedding, GW_CHILD);
	if (hDocObj == NULL)
		return 0;
	HWND hIEServer = ::FindWindowEx(hDocObj, NULL, CLASS_NAME_IE_SERVER, NULL);

	::SetFocus(hIEServer);
	return 0;
}

LRESULT CAxControl::OnDetachToMainThread(UINT, WPARAM, LPARAM, BOOL&)
{
	return 0;
	// 移到屏幕外面去搞，否则xp下桌面闪烁
	SetWindowPos(NULL, 30000, 30000, 0, 0, SWP_NOSIZE | SWP_HIDEWINDOW | SWP_NOACTIVATE);
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD dwNewStyle = (dwStyle & ~(WS_CHILD | WS_VISIBLE)) | (WS_POPUP | WS_DISABLED);
	::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);

	::SetParent(m_hWnd, NULL);
	AttachThreadInput(GetCurrentThreadId(), GetWindowThreadProcessId(m_hWnd, NULL), FALSE);
	return 0;
}

LRESULT CAxControl::OnOleCommand(UINT, WPARAM wparam, LPARAM lparam, BOOL&)
{
	if (m_spIE)
	{
		m_spIE->ExecWB((OLECMDID)wparam, (OLECMDEXECOPT)lparam, NULL, NULL);
	}
	return 0;
}

LRESULT CAxControl::OnAskFakeOwner(UINT, WPARAM, LPARAM, BOOL&)
{
	return LRESULT(CAxControl::GetTS() ? CAxControl::GetTS()->hFakeOwner : NULL);
}

LRESULT CAxControl::OnShowPupupOwned(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	HWND hh = CAxControl::GetTS() ? CAxControl::GetTS()->hFakeOwner : NULL;
	::ShowOwnedPopups(hh, wParam ? TRUE : FALSE);
	return 0;
}

LRESULT CAxControl::OnRegisterExternalCmd(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	return 0;
}

const AxControlConfig& CAxControl::GetConfig() const
{
	return m_Config;
}

LRESULT CAxControl::OnSetAxConfig(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	m_Config = *((AxControlConfig*)wParam);
	delete (AxControlConfig*)wParam;
	return 0;
}

LRESULT CAxControl::OnGetMarshalWebBrowser2CrossThread(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_spIE)
		return NULL;

	IStream* pStream = NULL;
	HRESULT hr = CoMarshalInterThreadInterfaceInStream(IID_IWebBrowser2, m_spIE, &pStream);
	if (FAILED(hr))
		return NULL;

	m_strReferer = (LPCWSTR)wParam;
	return (LRESULT)pStream;
}

LRESULT CAxControl::OnGetMarshalWebBrowser2CrossProcess(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_spIE)
		return NULL;

	ULONG ulSize = 0;
	if (FAILED(::CoGetMarshalSizeMax(&ulSize, IID_IWebBrowser2, m_spIE, MSHCTX_LOCAL, NULL, MSHLFLAGS_NORMAL)))
		return 0;

	if (wParam == -1)
		return ulSize;

	HGLOBAL hGlobal = ::GlobalAlloc(GHND, (DWORD)ulSize);
	if (hGlobal)
	{
		IStream *pStream = NULL;
		if (SUCCEEDED(::CreateStreamOnHGlobal(hGlobal, FALSE, &pStream)))
		{
			if (SUCCEEDED(CoMarshalInterface(pStream, IID_IWebBrowser2, m_spIE, MSHCTX_LOCAL, NULL, MSHLFLAGS_NORMAL)))
			{
				WCHAR szMappingName[128];
				wsprintfW(szMappingName, L"MarshalMapping_%d", lParam);
				HANDLE hMapping = ::OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, szMappingName);
				if (hMapping)
				{
					void *pData = ::MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
					if (pData)
					{
						memcpy(pData, ::GlobalLock(hGlobal), ulSize);
						::UnmapViewOfFile(pData);
					}
					::CloseHandle(hMapping);
				}
			}
		}
	}
	return ulSize;
}

LRESULT CAxControl::OnNetRedirect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPCWSTR szUrl = (LPCWSTR)wParam;
	LPCWSTR szRedirUrl = (LPCWSTR)lParam;

	free(((void*)wParam));
	free(((void*)lParam));
	return 0;
}

HRESULT CAxControl::GetBrowserObject(IOleObject** ppObject)
{
	if (m_spIEObj)
	{
		*ppObject = m_spIEObj;
		(*ppObject)->AddRef();
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CAxControl::GetBrowserObject(IWebBrowser2** ppWebBrowser)
{
	if (m_spIE)
	{
		*ppWebBrowser = m_spIE;
		(*ppWebBrowser)->AddRef();
		return S_OK;
	}
	return E_FAIL;
}

HWND CAxControl::GetFrameWnd()
{
	if (::IsWindow(m_hChildFrame))
		return m_hChildFrame;
	return NULL;
}
HWND CAxControl::GetFrameWndAndInitialized()
{
	if (!m_bInitialized)
		return NULL;

	return GetFrameWnd();
}

HRESULT CAxControl::GetHtmlDocument2(IHTMLDocument2** ppOut)
{
	if (m_spIE)
	{
		CComPtr<IDispatch> spDispDoc;
		if (SUCCEEDED(m_spIE->get_Document(&spDispDoc)) && spDispDoc)
			return spDispDoc->QueryInterface(IID_IHTMLDocument2, (void**)ppOut);
	}
	return E_FAIL;
}

LPCWSTR CAxControl::GetRefererUrl()
{
	return m_strReferer.c_str();
}

void CAxControl::SetURLBeforeNav(LPCWSTR szURL)
{
	m_strUrlBeforeNav = szURL;
}

bool CAxControl::IsRealNavigated()
{
	return m_dwRealNavigated == NAV_REAL;
}

LPCWSTR CAxControl::GetUrl()
{
	CComBSTR bstrURL;
	CComPtr<IHTMLDocument2> pDoc;
	GetHtmlDocument2(&pDoc);
	if (pDoc && SUCCEEDED(pDoc->get_URL(&bstrURL)))
		return bstrURL;
	return L"";
}

LPCWSTR CAxControl::GetLocationUrl()
{
	CComBSTR bstrURL;
	if (m_spIE)
	{
		m_spIE->get_LocationURL(&bstrURL);
		return bstrURL;
	}
	return L"";
}

class CShellEmbeddingHook : public CWindowImpl<CShellEmbeddingHook>
{

public:

	CShellEmbeddingHook(HWND hShellEmbedding, CAxControl *pAx) : m_pAx(pAx)
	{
		SubclassWindow(hShellEmbedding);
	}
	~CShellEmbeddingHook() {}

	void OnFinalMessage(HWND hwnd)
	{
		delete this;
	}

	BEGIN_MSG_MAP(CShellEmbeddingHook)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
	END_MSG_MAP()

	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = FALSE;
		WINDOWPOS *pos = (WINDOWPOS*)lParam;
		if ((pos->flags & SWP_NOSIZE) != SWP_NOSIZE)
			m_pAx->GetAxControlSize(&(pos->cx), &(pos->cy));
		if ((pos->flags & SWP_NOMOVE) != SWP_NOMOVE)
		{
			pos->x = 0;
			pos->y = 0;
		}
		return 0;
	}

private:
	CAxControl *m_pAx;
};

LRESULT CAxControl::WndProcHookIEThread(int iCode, WPARAM wParam, LPARAM lParam)
{
	if (iCode == HC_ACTION)
	{
		CWPSTRUCT *pcwp = (CWPSTRUCT*)lParam;
		if (!GetTS() || !GetTS()->pAxControl)
			return ::CallNextHookEx(NULL, iCode, wParam, lParam);
		CAxControl *pAx = GetTS()->pAxControl;
		if (pcwp->message == WM_CREATE)
		{
			CREATESTRUCT *pCS = (CREATESTRUCT*)lParam;
			wchar_t sz[128] = { 0 };
			::GetClassName(pcwp->hwnd, sz, _countof(sz));
			std::wstring strClassName = strClassName;
			if (strClassName == L"Shell Embedding")
			{
				// [TuotuoXP] 做这个hook目的是为了给ie6打patch，防止ie6自行缩放ie内核窗口的大小
				CREATESTRUCT *pcs = (CREATESTRUCT*)pcwp->lParam;
				if (pcs->hwndParent == pAx->m_hWnd)
					new CShellEmbeddingHook(pcwp->hwnd, pAx);
			}
		}
	}
	return ::CallNextHookEx(NULL, iCode, wParam, lParam);
}

LRESULT CAxControl::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostMessage(GetFrameWnd(), uMsg, wParam, lParam);
	return 0;
}

CBrowserSite* CAxControl::GetBroserSite() const
{
	return m_spBrowserSite;
}

CWebBrowserEventsManager* CAxControl::GetWebBrowserEventManager()
{
	return m_spBrowserEventManager;
}
