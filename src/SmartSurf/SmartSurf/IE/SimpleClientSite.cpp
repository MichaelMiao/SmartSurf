#include "stdafx.h"
#include "SimpleClientSite.h"
#include <DownloadMgr.h>
#include <mshtmcid.h>
#include <mshtmhst.h>
#include <mshtml.h>
#include "AxControl.h"
#include "BrowserEvents.h"

CBrowserSite::CBrowserSite(CAxControl* pOwner)
	: CRefCountBase("CBrowserSite"), m_pOwner(pOwner)
{
}

CBrowserSite::~CBrowserSite()
{
}

STDMETHODIMP CBrowserSite::QueryInterface(REFIID riid, void ** ppvObject)
{
	if (riid == IID_IUnknown)
	{
		*ppvObject = (void*)(this);
	}
	else if (riid == IID_IOleClientSite)
	{
		*ppvObject = (IOleClientSite*)(this);
	}
	else if (riid == IID_IDocHostUIHandler)
	{
		*ppvObject = (IDocHostUIHandler*)(this);
	}
	else if (riid == IID_IOleWindow)
	{
		*ppvObject = (IOleWindow*)(this);
	}
	else if (riid == IID_IOleInPlaceSite)
	{
		*ppvObject = (IOleInPlaceSite*)(this);
	}
	else if (riid == IID_IOleCommandTarget)
	{
		*ppvObject = (IOleCommandTarget*)(this);
	}
	else if (riid == IID_IServiceProvider)
	{
		*ppvObject = (IServiceProvider*)(this);
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

void CBrowserSite::SetOwner(CAxControl* pOwner)
{
	m_pOwner = pOwner;
}

HRESULT CBrowserSite::SaveObject()
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::GetContainer(IOleContainer **ppContainer)
{
	*ppContainer = NULL;
	return E_NOINTERFACE;
}

HRESULT CBrowserSite::ShowObject()
{
	return S_OK;
}

HRESULT CBrowserSite::OnShowWindow(BOOL fShow)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::RequestNewObjectLayout()
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::GetWindow(HWND *phwnd)
{
	TuoAssert(m_pOwner);
	if (m_pOwner)
	{
		*phwnd = m_pOwner->m_hWnd;
		return S_OK;
	}
	return E_UNEXPECTED;
}

HRESULT CBrowserSite::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::CanInPlaceActivate()
{
	return S_OK;
}

HRESULT CBrowserSite::OnInPlaceActivate()
{
	return S_OK;
}

HRESULT CBrowserSite::OnUIActivate()
{
	return S_OK;
}

HRESULT CBrowserSite::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	ATLASSERT(m_pOwner);
	HWND hWnd = NULL;
	GetWindow(&hWnd);

	*ppDoc = NULL;
	*ppFrame = NULL;
	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = hWnd;
	lpFrameInfo->haccel = 0;
	lpFrameInfo->cAccelEntries = 0;
	return S_OK;
}

HRESULT CBrowserSite::Scroll(SIZE scrollExtant)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::OnUIDeactivate(BOOL fUndoable)
{
	return S_OK;
}

HRESULT CBrowserSite::OnInPlaceDeactivate()
{
	return S_OK;
}

HRESULT CBrowserSite::DiscardUndoState()
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::DeactivateAndUndo()
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::OnPosRectChange(LPCRECT lprcPosRect)
{
	ATLASSERT(m_pOwner);
	if (m_pOwner)
	{
		CComPtr<IOleObject> spOleObj;
		m_pOwner->GetBrowserObject(&spOleObj);
		CComQIPtr<IOleInPlaceObject> spOleInPlaceObj = spOleObj;
		spOleInPlaceObj->SetObjectRects(lprcPosRect, lprcPosRect);
	}
	return S_OK;
}


struct CurrentWorkingIEContextMenuData
{
	WORD wMenuMask;
	int iInsertPosition;
	int iMenuCmdBegin;
	std::wstring strTargetURL;
	std::wstring strSelectedText;
	std::wstring strLinkHrefURL;
	std::wstring strLinkText;

	void Reset()
	{
		wMenuMask = 0;
		iInsertPosition = 0;
		iMenuCmdBegin = -1;
		strTargetURL.clear();
		strSelectedText.clear();
		strLinkHrefURL.clear();
		strLinkText.clear();
	}
};

CurrentWorkingIEContextMenuData g_CurrentContextMenu;

bool GetAnchorFromElement(IHTMLElement *pElement, IHTMLAnchorElement **ppAnchorElement)
{
	CComPtr<IHTMLElement> spHTMLSrcTemp = pElement;
	CComQIPtr<IHTMLAnchorElement> spHTMLAnchor = pElement;
	while (!spHTMLAnchor && spHTMLSrcTemp)
	{
		CComPtr<IHTMLElement> spParentElement;
		if (FAILED(spHTMLSrcTemp->get_parentElement(&spParentElement)))
			break;
		spHTMLAnchor = spParentElement;
		spHTMLSrcTemp = spParentElement;
	}
	*ppAnchorElement = spHTMLAnchor.Detach();
	return *ppAnchorElement != NULL;
}


// IDocHostUIHandler
HRESULT CBrowserSite::ShowContextMenu(DWORD dwID, POINT *pptScreen, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	if (m_pOwner->GetConfig().BeHideContextMenu)
		return S_OK;

	HRESULT hr = E_FAIL;
	return ShowContextMenu_Normal(dwID, pptScreen, pcmdtReserved, pdispReserved);
}

HRESULT CBrowserSite::ShowContextMenu_Normal(DWORD dwID, POINT *pptScreen, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	g_CurrentContextMenu.Reset();
	m_bIsIEServerPopupMenu = false;
	switch (dwID)
	{
	case CONTEXT_MENU_DEFAULT:
	{
		m_bIsIEServerPopupMenu = true;
		m_bstrClickRef = m_bstrClickText = NULL;
		if (pdispReserved)
		{
			CComPtr<IHTMLElement> pElem;
			HRESULT hr = pdispReserved->QueryInterface(IID_IHTMLElement, (void**)&pElem);
			if (SUCCEEDED(hr))
			{
				CComPtr<IHTMLDocument2> pDoc;
				hr = pElem->get_document((IDispatch**)&pDoc);
				if (SUCCEEDED(hr))
				{
					hr = pDoc->get_title(&m_bstrClickText);
					if (FAILED(hr))
						m_bstrClickText = NULL;
					hr = pDoc->get_URL(&m_bstrClickRef);
					if (FAILED(hr))
						m_bstrClickRef = NULL;
				}
			}
		}
	}
	break;

	case CONTEXT_MENU_ANCHOR:
	case CONTEXT_MENU_IMAGE:
	{
		CComQIPtr<IHTMLElement> pElem = pdispReserved;
		CComQIPtr<IHTMLAnchorElement> spAnchorElement;
		if (GetAnchorFromElement(pElem, &spAnchorElement))
		{
			CComBSTR bstrHref, bstrText;
			if (SUCCEEDED(spAnchorElement->get_href(&bstrHref)))
			{
				pElem->get_outerText(&bstrText);
				g_CurrentContextMenu.strLinkHrefURL = bstrHref ? bstrHref : L"";
				g_CurrentContextMenu.strLinkText = bstrText ? bstrText : L"";
			}
		}
	}
	break;
	}

	return E_NOTIMPL;
}

HRESULT CBrowserSite::ShowContextMenu_Flitered(DWORD dwID, POINT *pptScreen, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	// 显示菜单 或者 取消了选择
	bool bShowMenu = false;
	bool bClearSel = false;

	// 先检查是否是在编辑控件上
	if (dwID & CONTEXT_MENU_CONTROL)
	{
		// 向上找5级
		CComQIPtr<IHTMLElement> spCheck = pdispReserved;
		for (int i = 0; i < 5; ++i)
		{
			if (!spCheck)
				break;

			if (!bShowMenu)
			{
				CComQIPtr<IHTMLInputElement> spHTMLInput = spCheck;
				if (spHTMLInput)
					bShowMenu = true;
			}
			if (!bShowMenu)
			{
				CComQIPtr<IHTMLTextAreaElement> spHTMLTextArea = spCheck;
				if (spHTMLTextArea)
					bShowMenu = true;
			}

			if (bShowMenu)
				break;

			CComQIPtr<IHTMLElement> spParent;
			spCheck->get_parentElement(&spParent);
			if (spParent && !spParent.IsEqualObject(spCheck))
				spCheck = spParent;
		}
	}

	return bShowMenu ? E_NOTIMPL : S_OK;
}

HRESULT CBrowserSite::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
	pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE;
	return S_OK;
}


HRESULT CBrowserSite::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget,
	IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	return S_OK;
}

HRESULT CBrowserSite::HideUI(void)
{
	return S_OK;
}

HRESULT CBrowserSite::UpdateUI(void)
{
	return S_OK;
}

HRESULT CBrowserSite::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::OnDocWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::OnFrameWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
	return S_OK;
}

HRESULT CBrowserSite::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	return S_FALSE;
}

HRESULT CBrowserSite::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	return S_OK;
}

HRESULT CBrowserSite::GetExternal(IDispatch **ppDispatch)
{
	return S_OK;
}

HRESULT CBrowserSite::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	//OutputDebugString(L"TranslateUrl");
	//OutputDebugString(pchURLIn);

	if (_wcsnicmp(pchURLIn, L"res://", 6) == 0)
	{
		std::wstring strURL;
		if (!strURL.empty())
		{
			int nBytes = (strURL.size() + 1) * 2;
			*ppchURLOut = (OLECHAR*)CoTaskMemAlloc(nBytes);
			memset(*ppchURLOut, 0, nBytes);
			memcpy(*ppchURLOut, strURL.c_str(), strURL.size() * 2);
			//OutputDebugString(strURL.c_str());
		}
	}


	return S_OK;
}

HRESULT CBrowserSite::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD* prgCmds, OLECMDTEXT *pCmdText)
{
	return E_NOTIMPL;
}

HRESULT CBrowserSite::Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
{
	HRESULT hr = OLECMDERR_E_UNKNOWNGROUP;

	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
	{
		switch (nCmdID)
		{
		case OLECMDID_SHOWSCRIPTERROR:			// 屏蔽脚本错误的对话框
		{
			(*pvaOut).vt = VT_BOOL;
			(*pvaOut).boolVal = VARIANT_TRUE;
			return S_OK;
		}
		case IDM_REFRESH:	// webbrowser->refresh
		case 6041:	// F5
		case 6042:	// context menu
		case 6043:	// ctrl + F5
		{
			CWebBrowserEventsManager* pEventManager = m_pOwner->GetWebBrowserEventManager();
			if (pEventManager)
				pEventManager->OnRefreshEvent();
		}
		default:
			hr = OLECMDERR_E_NOTSUPPORTED;
			break;
		}
	}

	return hr;
}

HRESULT CBrowserSite::QueryService(REFGUID guidService, REFIID riid, void **ppvObject)
{
	return E_NOINTERFACE;
}

// [WuJian]


#define IDM_SEPARATOR                  64000
#define IDM_SAVE_FORM_DATA             64001
#define IDM_FILL_FORM_DATA             64002

//////////////////////////////////////////////////////////////////////////
// 处理ie8下面对右键的预处理

class CIECoreProcess : public CWindowImpl<CIECoreProcess>
{

public:

	CIECoreProcess(bool bIsEditMenu) : m_bIsEditMenu(bIsEditMenu) {}

	BEGIN_MSG_MAP(CIECoreProcess)
		MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenuPopup)
	END_MSG_MAP()

	LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		DefWindowProc();
		if (m_bIsEditMenu)
		{
			::EnableMenuItem((HMENU)wParam, IDM_SAVE_FORM_DATA, MF_BYCOMMAND | MF_ENABLED);
			::EnableMenuItem((HMENU)wParam, IDM_FILL_FORM_DATA, MF_BYCOMMAND | MF_ENABLED);
			::EnableMenuItem((HMENU)wParam, IDM_FILL_FORM_DATA, MF_BYCOMMAND | MF_ENABLED);
		}
		else
		{
			::EnableMenuItem((HMENU)wParam, IDM_ADDFAVORITES, MF_BYCOMMAND | MF_ENABLED);
			::EnableMenuItem((HMENU)wParam, IDM_CREATESHORTCUT, MF_BYCOMMAND | MF_ENABLED);
		}
		return 0;
	}

	bool m_bIsEditMenu;
};

extern BOOL(WINAPI * Real_TrackPopupMenuEx)(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm);
BOOL WINAPI Mine_TrackPopupMenuEx_ContextMenu(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm)
{
	return FALSE;
}
