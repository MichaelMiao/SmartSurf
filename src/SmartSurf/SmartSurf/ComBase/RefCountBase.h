#pragma once
#include <atlcomcli.h>


/************************************************************************/
/*
CRefCountBase用于管理使用引用计数的类的声明周期，使用方法为：

class CTest : public CRefCountBase
{
	...
	ULONG STDMETHODCALLTYPE AddRef() { return CRefCountBase::IncreaseRefCount(); }
	ULONG STDMETHODCALLTYPE Release() { return CRefCountBase::DecreaseRefCount(); }
	...
};

创建CTest的方式有如下几种：

1. 如果CTest的指针保存在类的成员变量里面，则使用CreateComInstance来创建，如：
	m_pTest = CreateComInstance<CTest>(...);
2. 如果CTest在栈上使用，且要交给其他的模块处理，则可以使用CComPtr
	CComPtr<CTest> spTest = new CTest(...);
	...

* 如果有一个类实现IDispatch，可以使用CDispatchBase
* 如果有一个类实现IDispatch且被advise，则可以使用CAdvisableBase

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
	// 引用计数不允许赋值和拷贝
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
