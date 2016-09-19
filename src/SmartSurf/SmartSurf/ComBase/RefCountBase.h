#pragma once
#include <atlcomcli.h>


/************************************************************************/
/*
CRefCountBase���ڹ���ʹ�����ü���������������ڣ�ʹ�÷���Ϊ��

class CTest : public CRefCountBase
{
	...
	ULONG STDMETHODCALLTYPE AddRef() { return CRefCountBase::IncreaseRefCount(); }
	ULONG STDMETHODCALLTYPE Release() { return CRefCountBase::DecreaseRefCount(); }
	...
};

����CTest�ķ�ʽ�����¼��֣�

1. ���CTest��ָ�뱣������ĳ�Ա�������棬��ʹ��CreateComInstance���������磺
	m_pTest = CreateComInstance<CTest>(...);
2. ���CTest��ջ��ʹ�ã���Ҫ����������ģ�鴦�������ʹ��CComPtr
	CComPtr<CTest> spTest = new CTest(...);
	...

* �����һ����ʵ��IDispatch������ʹ��CDispatchBase
* �����һ����ʵ��IDispatch�ұ�advise�������ʹ��CAdvisableBase

*/
/************************************************************************/
#define UNKNOWNSUPPORT\
	ULONG STDMETHODCALLTYPE AddRef() { return CRefCountBase::IncreaseRefCount(); }\
	ULONG STDMETHODCALLTYPE Release() { return CRefCountBase::DecreaseRefCount(); }\
	STDMETHODIMP QueryInterface(REFIID iid, void ** ppvObject);

class CRefCountBase
{
public:
	CRefCountBase(LPCSTR szObj);
	virtual ~CRefCountBase();

	// 
	ULONG IncreaseRefCount();
	ULONG DecreaseRefCount();
	ULONG GetRefCount();

private:
	// ���ü���������ֵ�Ϳ���
	CRefCountBase(CRefCountBase&);
	const CRefCountBase& operator = (const CRefCountBase&);

private:
	ULONG m_uRef;
#ifdef _DEBUG
	std::string m_strObjName;
#endif
};

//////////////////////////////////////////////////////////////////////////
template<class CObjType>
CObjType* CreateComInstance()
{
	CObjType* pNewInstance = new CObjType();	
// 	pNewInstance->AddRef();
	return pNewInstance;
}

template<class CObjType, class CParam0>
CObjType* CreateComInstance(CParam0 param0)
{
	CObjType* pNewInstance = new CObjType(param0);	
// 	pNewInstance->AddRef();
	return pNewInstance;
}

template<class CObjType, class CParam0, class CParam1>
CObjType* CreateComInstance(CParam0 param0, CParam1 param1)
{
	CObjType* pNewInstance = new CObjType(param0, param1);	
// 	pNewInstance->AddRef();
	return pNewInstance;
}

template <class T>
class CComObjPtr : 
	public CComPtrBase<T>
{
public:
	CComObjPtr() throw()
	{
	}
	CComObjPtr(_Inout_opt_ T* lp) throw() :	CComPtrBase<T>(lp)
	{
	}
	CComObjPtr(_Inout_ const CComObjPtr<T>& lp) throw() : CComPtrBase<T>(lp.p)
	{	
	}
	T* operator=(_Inout_opt_ T* lp) throw()
	{
		if(*this!=lp)
		{
			if (lp != NULL)
				lp->AddRef();
			if (p)
				p->Release();
			p = lp;
		}
		return *this;
	}
	T* operator=(_Inout_ const CComObjPtr<T>& lp) throw()
	{
		if(*this!=lp)
		{
			if (lp != NULL)
				lp->AddRef();
			if (p)
				p->Release();
			p = lp;
		}
		return *this;
	}	
	CComObjPtr(_Inout_ CComObjPtr<T>&& lp) throw() :	
		CComPtrBase<T>()
	{	
		p = lp.p;		
		lp.p = NULL;
	}	
	T* operator=(_Inout_ CComObjPtr<T>&& lp) throw()
	{			
		if (*this != lp)
		{
			if (p != NULL)			
				p->Release();

			p = lp.p;
			lp.p = NULL;
		}
		return *this;		
	}
};
