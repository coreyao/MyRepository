#include "Network/iocp/IOCompletionPort.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CIOCompletionPort::CIOCompletionPort()
: m_uThreadCount(4)
{
	m_IOCP = CreateIOCP(m_uThreadCount);
}

CIOCompletionPort::CIOCompletionPort(unsigned int uThreadCount)
: m_uThreadCount(uThreadCount)
{
	m_IOCP = CreateIOCP(m_uThreadCount);
}

CIOCompletionPort::~CIOCompletionPort()
{
	ReleaseIOCP(m_IOCP);
}

uint32 CIOCompletionPort::GetIOCPThreadCount() const
{
	return m_uThreadCount;
}

bool CIOCompletionPort::AssociateDevice(HANDLE hDevice, ULONG_PTR completionKey)
{
	if (m_IOCP != ::CreateIoCompletionPort(hDevice, m_IOCP, completionKey, 0))
	{
		return false;
	}

	return true;
}

bool CIOCompletionPort::PostStatus(ULONG_PTR completionKey, DWORD dwNumBytes, OVERLAPPED* pOverlapped)
{
	if (::PostQueuedCompletionStatus(m_IOCP, dwNumBytes, completionKey, pOverlapped) == 0)
	{
		return false;
	}

	return true;
}

bool CIOCompletionPort::GetStatus(ULONG_PTR* pCompletionKey, DWORD* pdwNumBytes, OVERLAPPED** ppOverlapped, DWORD dwMilliseconds /* = INFINITE */)
{
	if (::GetQueuedCompletionStatus(m_IOCP, pdwNumBytes, pCompletionKey, ppOverlapped, dwMilliseconds) == 0)
	{
		return false;
	}

	return true;
}

HANDLE CIOCompletionPort::CreateIOCP(uint32 uThreadCount)
{
	return ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, uThreadCount);
}

void CIOCompletionPort::ReleaseIOCP(HANDLE hIOCP)
{
	::CloseHandle(hIOCP);
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)