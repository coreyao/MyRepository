#ifndef CORE_UTILITY_NETWORK_IOCP_IOCPOBJECT_H__INCLUDED
#define CORE_UTILITY_NETWORK_IOCP_IOCPOBJECT_H__INCLUDED

#include "Network/iocp/IOCompletionPort.h"
#include "Network/OverlappedBuff.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CIOCPObject
{
public:
	CIOCPObject();
	virtual ~CIOCPObject();

	void SetBindIOCP(CIOCompletionPort* iocp);
	CIOCompletionPort* GetBindIOCP() const;

	virtual void OnIOCompleteSuccess(COverLappedBuffer* pIOBuff) = 0;
	virtual void OnIOCompleteFailed(COverLappedBuffer* pIOBuff) = 0;

protected:
	CIOCompletionPort* m_bindiocp;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif