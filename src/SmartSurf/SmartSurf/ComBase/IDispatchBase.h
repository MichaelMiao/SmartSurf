// author: TuotuoXP
#pragma once
#include "RefCountBase.h"


class CDispatchBase : public IDispatch, public CRefCountBase
{
public:
	CDispatchBase(LPCSTR szObj);
	virtual ~CDispatchBase();

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef() { return CRefCountBase::IncreaseRefCount(); }
	ULONG STDMETHODCALLTYPE Release() { return CRefCountBase::DecreaseRefCount(); }

	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo);
	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

private:
	virtual DISPID OnGetIDOfName(LPCWSTR lpszName) = 0;
	virtual bool OnInvoke(DISPID dispIdMember, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult) = 0;
};
