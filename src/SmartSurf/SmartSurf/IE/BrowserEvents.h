#pragma once
#include "../ComBase/AdvisableBase.h"
#include <exdisp.h>

class CAxControl;
class CWebBrowserEventsManager : public CAdvisableBase<DIID_DWebBrowserEvents2>
{
public:
	explicit CWebBrowserEventsManager(CAxControl* pOwner);
	~CWebBrowserEventsManager();

	void SetOwner(CAxControl* pOwner);

public:

	bool OnInvoke(DISPID dispIdMember, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult) override;

public:
	void OnRefreshEvent();
	void OnWndCloseEvent();

private:
	void SetRefresh(bool bRefresh);
	void SetPercentage(DWORD dwNew, bool bForce);

private:
	HRESULT OnTitleChange(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnStatuesChange(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnBeforeNavigate2(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnNavigateComplete2(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnDocumentComplete(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnProgressChange(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnCommandStateChange(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnNavigateError(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnNewWindow(LPCWSTR lpszUrl, LPCWSTR lpszReferrer, IDispatch **&ppDisp, VARIANT_BOOL *&Cancel, bool bForceNewWindow = false);
	HRESULT OnNewWindow2(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnNewWindow3(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnDownloadBegin(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	HRESULT OnDownloadComplete(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult);
	BOOL ForceNewWindow(DISPPARAMS* pDispParams);

private:

	CAxControl *m_pOwner;
	bool m_bShutDownAndDestroyWnd;
	IDispatch *m_pDispError;
	DWORD m_dwCookie;
	DWORD m_dwPercentage;

	bool m_bRefresh;
	std::wstring m_wstrRef;
	std::wstring m_strUrl;
};
