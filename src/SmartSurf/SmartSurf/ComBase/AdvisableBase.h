// author: TuotuoXP
#pragma once
#include "IDispatchBase.h"


template <const IID &iid>
class CAdvisableBase : public CDispatchBase
{
public:
	explicit CAdvisableBase(LPCSTR szObj) : CDispatchBase(szObj), m_dwCookie(0), m_pCP(NULL) {}
	virtual ~CAdvisableBase() {}


	bool Advise(IUnknown *pUnkn)
	{
		if (m_dwCookie == 0)
		{
			if (SUCCEEDED(::AtlAdvise(pUnkn, static_cast<IUnknown*>(this), iid, &m_dwCookie)))
			{
				m_pCP = pUnkn;
				m_pCP->AddRef();
				return true;
			}
		}
		return false;
	}

	void Unadvise()
	{
		if (m_dwCookie != 0)
		{
			::AtlUnadvise(m_pCP, iid, m_dwCookie);
			m_pCP->Release();
			m_pCP = NULL;
			m_dwCookie = 0;
		}
	}

	virtual bool OnInvoke(DISPID dispIdMember, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult) = 0;

private:

	DISPID OnGetIDOfName(LPCWSTR lpszName) override { return DISPID_UNKNOWN; }


	DWORD m_dwCookie;
	IUnknown *m_pCP;
};
