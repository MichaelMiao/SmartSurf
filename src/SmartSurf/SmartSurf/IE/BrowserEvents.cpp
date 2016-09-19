#include "stdafx.h"
#include "BrowserEvents.h"

CWebBrowserEventsManager::CWebBrowserEventsManager(CAxControl* pOwner)
	: CAdvisableBase<DIID_DWebBrowserEvents2>("CWebBrowserEventsManager"),
	m_pOwner(pOwner)
{

}

CWebBrowserEventsManager::~CWebBrowserEventsManager()
{

}

bool CWebBrowserEventsManager::OnInvoke(DISPID dispIdMember, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult)
{
	return true;
}

void CWebBrowserEventsManager::OnRefreshEvent()
{

}

void CWebBrowserEventsManager::OnWndCloseEvent()
{

}

void CWebBrowserEventsManager::SetRefresh(bool bRefresh)
{

}

void CWebBrowserEventsManager::SetPercentage(DWORD dwNew, bool bForce)
{

}

HRESULT CWebBrowserEventsManager::OnTitleChange(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnStatuesChange(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnBeforeNavigate2(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnNavigateComplete2(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnDocumentComplete(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnProgressChange(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnCommandStateChange(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnNavigateError(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnNewWindow(LPCWSTR lpszUrl, LPCWSTR lpszReferrer, IDispatch **&ppDisp, VARIANT_BOOL *&Cancel, bool bForceNewWindow /*= false*/)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnNewWindow2(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnNewWindow3(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnDownloadBegin(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

HRESULT CWebBrowserEventsManager::OnDownloadComplete(WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult)
{
	return S_OK;
}

BOOL CWebBrowserEventsManager::ForceNewWindow(DISPPARAMS* pDispParams)
{
	return S_OK;
}

void CWebBrowserEventsManager::SetOwner(CAxControl* pOwner)
{
	m_pOwner = pOwner;
}
