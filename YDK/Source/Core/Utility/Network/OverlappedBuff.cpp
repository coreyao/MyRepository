#include "Network/OverlappedBuff.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

COverLappedBuffer::COverLappedBuffer( uint32 unMaxBufferSize /*= SO_MAX_MSG_SIZE*/ )
: m_bUsed(false)
, m_uiNumberOfBytes(0)
, m_iOper(0)
, m_uiMaxBufferSize(unMaxBufferSize)
, m_pszBuff(NULL)
{
	m_pszBuff = new char[m_uiMaxBufferSize];
	m_wsaBuff.buf = m_pszBuff;
	m_wsaBuff.len = 0;
	memset(static_cast<LPOVERLAPPED>(this), 0, sizeof(OVERLAPPED));
}

COverLappedBuffer::~COverLappedBuffer()
{
	if (m_pszBuff != NULL)
	{
		delete [] m_pszBuff;
		m_pszBuff = NULL;
	}
}

WSABUF* COverLappedBuffer::GetWSABuff()
{
	return &m_wsaBuff;
}

char* COverLappedBuffer::GetBuffer()
{
	return m_wsaBuff.buf;
}

bool COverLappedBuffer::IsEmpty() const
{
	return GetUsedLength() == 0;
}

uint32 COverLappedBuffer::GetUsedLength() const
{
	return m_wsaBuff.len;
}

void COverLappedBuffer::SetUsedLength( uint32 uLength )
{
	m_wsaBuff.len = uLength;
}

uint32 COverLappedBuffer::GetMaxBuffSize() const
{
	return m_uiMaxBufferSize;
}

void COverLappedBuffer::SetMaxBuffSize( uint32 uiMaxBuffSize )
{
	if (m_uiMaxBufferSize == uiMaxBuffSize)
	{
		return;
	}

	if (m_pszBuff != NULL)
	{
		delete [] m_pszBuff;
		m_pszBuff = NULL;
	}

	m_uiMaxBufferSize = uiMaxBuffSize;
	m_pszBuff = new char[m_uiMaxBufferSize];
}

void COverLappedBuffer::SetUsed( bool bUsed )
{	
	m_bUsed = bUsed;
}

bool COverLappedBuffer::IsUsed() const
{
	return m_bUsed;
}

void COverLappedBuffer::SetOper( int32 oper )
{
	m_iOper = oper;
}

int32 COverLappedBuffer::GetOper() const
{
	return m_iOper;
}

bool COverLappedBuffer::PopData( uint32 uiLength )
{
	if (uiLength == 0)
	{
		return false;
	}

	uint32 uCurrentLength = GetUsedLength();
	if (uCurrentLength < uiLength)
	{
		return false;
	}

	if (uCurrentLength == uiLength)
	{
		Clear();
		return true;
	}

	m_wsaBuff.buf += uiLength;
	SetUsedLength(uCurrentLength - uiLength);

	return true;
}

uint32 COverLappedBuffer::PushData( const char* pData, uint32 uiLength )
{
	uint32 uCurrentLength = GetUsedLength();
	uint32 uSize = m_uiMaxBufferSize - uCurrentLength;

	if (pData == NULL || uiLength == 0)
	{
		return 0;
	}

	if (uiLength > uSize)
	{
		uiLength = uSize;
	}

	memcpy(&m_wsaBuff.buf[uCurrentLength], pData, uiLength);
	SetUsedLength(uCurrentLength + uiLength);

	return uiLength;
}

void COverLappedBuffer::Clear()
{
	m_wsaBuff.buf = m_pszBuff;
	m_wsaBuff.len = 0;
}

DWORD COverLappedBuffer::GetNumberOfBytes() const
{
	return m_uiNumberOfBytes;
}

void COverLappedBuffer::SetNumberOfBytes( DWORD dwNumberOfBytes )
{
	m_uiNumberOfBytes = dwNumberOfBytes;
}

void COverLappedBuffer::SwapData( COverLappedBuffer& iobuff )
{
	if (iobuff.GetMaxBuffSize() != GetMaxBuffSize())
	{
		return;
	}

	char* szBuff = iobuff.m_pszBuff;
	iobuff.m_pszBuff = m_pszBuff;
	m_pszBuff = szBuff;

	DWORD numberOfBytes = iobuff.m_uiNumberOfBytes;
	iobuff.m_uiNumberOfBytes = m_uiNumberOfBytes;
	m_uiNumberOfBytes = numberOfBytes;

	WSABUF wsaBuff = iobuff.m_wsaBuff;
	iobuff.m_wsaBuff = m_wsaBuff;
	m_wsaBuff = wsaBuff;
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

