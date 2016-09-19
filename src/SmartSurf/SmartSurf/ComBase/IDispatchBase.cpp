#include "stdafx.h"
#include "IDispatchBase.h"


CDispatchBase::CDispatchBase(LPCSTR szObj) : CRefCountBase(szObj) {}
CDispatchBase::~CDispatchBase() {}


HRESULT CDispatchBase::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == __uuidof(IUnknown))
	{
		*ppvObject = static_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}
	else if (riid == __uuidof(IDispatch))
	{
		*ppvObject = static_cast<IDispatch*>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}


HRESULT CDispatchBase::GetTypeInfoCount(UINT *pctinfo)
{
	return S_OK;
}
HRESULT CDispatchBase::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return S_OK;
}

HRESULT CDispatchBase::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	HRESULT hRes = S_OK;
	for (UINT i = 0; i < cNames; i++)
	{
		rgDispId[i] = OnGetIDOfName(rgszNames[i]);
		if (rgDispId[i] == DISPID_UNKNOWN)
			hRes = DISP_E_UNKNOWNNAME;
	}
	return hRes;
}

HRESULT CDispatchBase::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	if (dispIdMember == DISPID_VALUE && (wFlags & DISPATCH_PROPERTYGET) != 0 && pVarResult)
	{
		::VariantInit(pVarResult);
		pVarResult->vt = VT_BSTR;
		pVarResult->bstrVal = ::SysAllocString(L"[Sogou Explorer Object]");
		return S_OK;
	}
	else if (dispIdMember != DISPID_UNKNOWN)
	{
		if (OnInvoke(dispIdMember, wFlags, pDispParams, pVarResult))
			return S_OK;
	}
	if (pVarResult)
		pVarResult->vt = VT_EMPTY;
	return S_OK;
}
