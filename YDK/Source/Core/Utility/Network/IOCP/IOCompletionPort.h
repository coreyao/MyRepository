#ifndef CORE_UTILITY_NETWORK_IOCP_IOCOMPLETIONPORT_H__INCLUDED
#define CORE_UTILITY_NETWORK_IOCP_IOCOMPLETIONPORT_H__INCLUDED

#include <WinSock2.h>

#include "BasicDefines.h"
#include "UtilityTypes.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CIOCompletionPort
{
public:
	CIOCompletionPort();
	CIOCompletionPort(unsigned int uThreadCount);

	virtual ~CIOCompletionPort();

	unsigned int GetIOCPThreadCount() const;

	bool AssociateDevice(HANDLE hDevice, ULONG_PTR completionKey);
	bool PostStatus(ULONG_PTR completionKey, DWORD dwNumBytes, OVERLAPPED* pOverlapped);
	bool GetStatus(ULONG_PTR* pCompletionKey, DWORD* pdwNumBytes, OVERLAPPED** ppOverlapped, DWORD dwMilliseconds = INFINITE);

protected:
	HANDLE CreateIOCP(uint32 uThreadCount);
	void ReleaseIOCP(HANDLE hIOCP);

	HANDLE m_IOCP;
	unsigned int m_uThreadCount;
};


END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif