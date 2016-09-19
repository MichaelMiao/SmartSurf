#include "stdafx.h"
#include "RefCountBase.h"


CRefCountBase::CRefCountBase(LPCSTR szObj) : m_uRef(0)
{
#ifdef _DEBUG
//  	m_strObjName = szObj;
// 	std::string strOut = "+++";
// 	strOut += szObj;
// 	strOut += "\r\n";
// 	::OutputDebugStringA(strOut.c_str());
#endif
}

CRefCountBase::~CRefCountBase()
{
#ifdef _DEBUG
// 	std::string strOut = "---";
// 	strOut += m_strObjName;
// 	strOut += "\r\n";
// 	::OutputDebugStringA(strOut.c_str());
#endif
}

ULONG CRefCountBase::IncreaseRefCount()
{
	return ++m_uRef;
}

ULONG CRefCountBase::DecreaseRefCount()
{
	TuoAssert(m_uRef != 0);
	if (--m_uRef == 0)
	{
		delete this;
		return 0;
	}
	return m_uRef;
}

ULONG CRefCountBase::GetRefCount()
{
	return m_uRef;
}
