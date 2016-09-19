#pragma once
#include <mshtmhst.h>
#include "../ComBase/RefCountBase.h"


class CAxControl;
class CBrowserSite
	: public CRefCountBase
	, public IOleClientSite
	, public IOleInPlaceSite
	, public IDocHostUIHandler
	, public IOleCommandTarget
	, public IServiceProvider
{
public:

	explicit CBrowserSite(CAxControl* pOwner);
	~CBrowserSite();

	void SetOwner(CAxControl* pOwner);

public:
	UNKNOWNSUPPORT

		// IOleClientSite
		HRESULT STDMETHODCALLTYPE SaveObject();
	HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
	HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer);
	HRESULT STDMETHODCALLTYPE ShowObject();
	HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
	HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();

	// IOleWindow
	HRESULT STDMETHODCALLTYPE GetWindow(HWND *phwnd);
	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);

	// IOleInPlaceSite : IOleWindow
	HRESULT STDMETHODCALLTYPE CanInPlaceActivate();
	HRESULT STDMETHODCALLTYPE OnInPlaceActivate();
	HRESULT STDMETHODCALLTYPE OnUIActivate();
	HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant);
	HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate();
	HRESULT STDMETHODCALLTYPE DiscardUndoState();
	HRESULT STDMETHODCALLTYPE DeactivateAndUndo();
	HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect);

	//IDocHostUIHandler
	HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT *pptScreen, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	HRESULT STDMETHODCALLTYPE GetHostInfo(DOCHOSTUIINFO *pInfo);
	HRESULT STDMETHODCALLTYPE ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc);
	HRESULT STDMETHODCALLTYPE HideUI(void);
	HRESULT STDMETHODCALLTYPE UpdateUI(void);
	HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
	HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL fActivate);
	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL fActivate);
	HRESULT STDMETHODCALLTYPE ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
	HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
	HRESULT STDMETHODCALLTYPE GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw);
	HRESULT STDMETHODCALLTYPE GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	HRESULT STDMETHODCALLTYPE GetExternal(IDispatch **ppDispatch);
	HRESULT STDMETHODCALLTYPE TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
	HRESULT STDMETHODCALLTYPE FilterDataObject(IDataObject *pDO, IDataObject **ppDORet);

	//IOleCommandTarget
	STDMETHOD(QueryStatus)(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD* prgCmds, OLECMDTEXT *pCmdText);
	STDMETHOD(Exec)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);

	//IServiceProvider
	HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService, REFIID riid, void **ppvObject);

private:

	HRESULT ShowContextMenu_Normal(DWORD dwID, POINT *pptScreen, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	HRESULT ShowContextMenu_Flitered(DWORD dwID, POINT *pptScreen, IUnknown *pcmdtReserved, IDispatch *pdispReserved);

public:

	CAxControl* m_pOwner;

	bool m_bIsIEServerPopupMenu;		// [WuJian] 用来判断是IE的右键菜单还是Flash的右键菜单
	BSTR m_bstrClickRef;
	BSTR m_bstrClickText;
};
