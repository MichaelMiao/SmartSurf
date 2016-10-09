#pragma once
#include <algorithm>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


namespace base
{
template<class char_type> struct Printfs
{
	inline Printfs(const char_type *format, ...);
	char_type szBuf[64];
	int iLen;
};

Printfs<char>::Printfs(const char *format, ...)
{
	va_list va;
	va_start(va, format);
	iLen = vsprintf_s(szBuf, format, va);
	va_end(va);
}

Printfs<wchar_t>::Printfs(const wchar_t *format, ...)
{
	va_list va;
	va_start(va, format);
	iLen = vswprintf_s(szBuf, format, va);
	va_end(va);
}

typedef Printfs<char> PrintfsA;
typedef Printfs<wchar_t> PrintfsW;

template<class t_char> struct StrWithLen
{
	StrWithLen(const t_char *s, size_t len) : pData(s), szLen(len) {}
	const t_char *pData;
	size_t szLen;
};

template <typename t_char>
size_t strLen(const t_char* lpszString);

template <>
size_t strLen<char>(const char* lpszString) { return strlen(lpszString); }

template <>
size_t strLen<wchar_t>(const wchar_t* lpszString) { return wcslen(lpszString); }

template <typename t_char>
class base_stringbuf
{
	typedef base_stringbuf<t_char> stringbuf_t;

public:

	base_stringbuf() : m_pData(m_DataInit){}
	size_t len() const { return m_szLen; }
	size_t cap() const { return m_szCap; }
	t_char* data() const { return m_pData; }

	friend stringbuf_t& operator << (stringbuf_t& leftBuf, stringbuf_t& rightBuf)
	{
		size_t szNewLen = leftBuf.len() + rightBuf.len() + 1;
		if (szNewLen > leftBuf.m_szCap)
			leftBuf.ExpandCap(szNewLen);

		memcpy(leftBuf.data() + leftBuf.len(), rightBuf.data(), sizeof(t_char) * rightBuf.len());
		leftBuf.m_szLen = leftBuf.len() + rightBuf.len();
	}
	
	friend stringbuf_t& operator << (stringbuf_t& leftBuf, const StrWithLen<t_char>& str)
	{
		if (str.pData && str.pData[0])
		{
			const size_t szLen = str.szLen;
			size_t szNewLen = leftBuf.len() + szLen + 1;
			if (szNewLen > leftBuf.m_szCap)
				leftBuf.ExpandCap(szNewLen);

			memcpy(leftBuf.data() + leftBuf.len(), str.pData, sizeof(t_char) * szLen);
			leftBuf.m_szLen = leftBuf.len() + szLen;
		}
		return leftBuf;
	}

	friend stringbuf_t& operator << (stringbuf_t& leftBuf, const t_char* lpszRight)
	{
		return leftBuf << StrWithLen<t_char>(lpszRight, strLen(lpszRight));
	}

	friend stringbuf_t& operator << (stringbuf_t& leftBuf, const std::basic_string<t_char> strRight)
	{
		return leftBuf << StrWithLen<t_char>(strRight.c_str(), strRight.length());
	}

	friend inline stringbuf_t& operator << (stringbuf_t& sbLeft, const Printfs<t_char>& printfs)
	{
		return sbLeft << StrWithLen<t_char>(printfs.szBuf, printfs.iLen);
	}

	friend inline stringbuf_t& operator << (stringbuf_t& sbLeft, const t_char cRight)
	{
		return sbLeft << StrWithLen<t_char>(&cRight, 1);
	}

private:

	void ExpandCap(const size_t szCap)
	{
		size_t szNew = std::max<size_t>(m_szLen * 2, m_szCap);
		const size_t szBufSize = szNew * sizeof(t_char);
		if (m_DataInit == m_pData)
		{
			m_pData = (t_char*)malloc(szBufSize);
			memcpy(m_pData, m_DataInit, szBufSize);
		}
		else
		{
			m_pData = (t_char*)realloc(m_pData, szBufSize);
		}
		m_szCap = szNew;
	}

	base_stringbuf(const base_stringbuf& buf);
	base_stringbuf& operator = (const base_stringbuf& buf);

	size_t		m_szLen = 0;
	size_t		m_szCap = 512;
	t_char*		m_pData = nullptr;
	t_char		m_DataInit[512];
};

typedef base_stringbuf<char> stringbuf;
typedef base_stringbuf<wchar_t> wstringbuf;

inline wstringbuf& operator << (wstringbuf& leftBuf, const char cValue)
{
	return leftBuf << (wchar_t)cValue;
}

inline stringbuf& operator << (stringbuf& leftBuf, const long lValue)
{
	char lpszRight[32];
	_ltoa_s(lValue, lpszRight, 32, 10);
	return leftBuf << lpszRight;
}
inline wstringbuf& operator << (wstringbuf& leftBuf, const long lValue)
{
	wchar_t lpszRight[32];
	_ltow_s(lValue, lpszRight, 32, 10);
	return leftBuf << lpszRight;
}

inline stringbuf& operator << (stringbuf& leftBuf, const unsigned long lValue)
{
	char lpszRight[32];
	_ultoa_s(lValue, lpszRight, 32, 10);
	return leftBuf << lpszRight;
}
inline wstringbuf& operator << (wstringbuf& leftBuf, const unsigned long lValue)
{
	wchar_t lpszRight[32];
	_ultow_s(lValue, lpszRight, 32, 10);
	return leftBuf << lpszRight;
}

inline stringbuf& operator << (stringbuf& leftBuf, const int iValue)
{
	char lpszRight[32];
	_itoa_s(iValue, lpszRight, 32, 10);
	return leftBuf << lpszRight;
}
inline wstringbuf& operator << (wstringbuf& leftBuf, const int iValue)
{
	wchar_t lpszRight[32];
	_itow_s(iValue, lpszRight, 32, 10);
	return leftBuf << lpszRight;
}

inline stringbuf& operator << (stringbuf& leftBuf, const unsigned int iValue)
{
	char lpszRight[32];
	_ultoa_s(iValue, lpszRight, 32, 10);
	return leftBuf << lpszRight;
}
inline wstringbuf& operator << (wstringbuf& leftBuf, const unsigned int iValue)
{
	wchar_t lpszRight[32];
	_ultow_s(iValue, lpszRight, 32, 10);
	return leftBuf << lpszRight;
}

inline stringbuf& operator << (stringbuf& leftBuf, const unsigned __int64 &iValue)
{
	return leftBuf << PrintfsA("%I64u", iValue);
}
inline wstringbuf& operator << (wstringbuf& leftBuf, const unsigned __int64 &iValue)
{
	return leftBuf << PrintfsW(L"%I64u", iValue);
}
inline stringbuf& operator << (stringbuf& leftBuf, const __int64 &iValue)
{
	return leftBuf << PrintfsA("%I64d", iValue);
}
inline wstringbuf& operator << (wstringbuf& leftBuf, const __int64 &iValue)
{
	return leftBuf << PrintfsW(L"%I64d", iValue);
}
}
