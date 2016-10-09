#pragma once


template <typename t_char>
class base_stringbuf
{
	typedef base_stringbuf<t_char> stringbuf_t;

public:

	base_stringbuf() : m_pData(m_DataInit){}
	size_t len() const { return m_szLen; }
	size_t cap() const { return m_szCap; }
	t_char* data() const { return m_pData; }

	stringbuf_t& operator << (stringbuf_t& leftBuf, const stringbuf_t& rightBuf)
	{
		size_t szNewLen = leftBuf.len() + rightBuf.len() + 1;
		if (szNewLen > m_szCap)
			ExpandCap(szNewLen);

		memcpy(leftBuf.data() + leftBuf.len(), rightBuf.data(), sizeof(t_char) * rightBuf.len());
		m_szLen = szNewLen;
	}

private:

	void ExpandCap(const size_t szCap)
	{
		size_t szNew = std::max(m_szLen * 2, m_szCap);
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

	const size_t BLOCK_SIZE = 512;
	size_t		m_szLen = 0;
	size_t		m_szCap = BLOCK_SIZE;
	t_char*		m_pData = nullptr;
	t_char		m_DataInit[BLOCK_SIZE];
};