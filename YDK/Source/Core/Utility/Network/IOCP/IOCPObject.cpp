#include "Network/iocp/IOCPObject.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CIOCPObject::CIOCPObject()
: m_bindiocp(NULL)
{

}

CIOCPObject::~CIOCPObject()
{

}

void CIOCPObject::SetBindIOCP(CIOCompletionPort *iocp)
{
	m_bindiocp = iocp;
}

CIOCompletionPort* CIOCPObject::GetBindIOCP() const
{
	return m_bindiocp;
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)