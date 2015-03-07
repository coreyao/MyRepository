#include "Network/Worker/NetworkListener.h"
#include "Network/Session/ServerSession.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CNetworkListener::CNetworkListener()
{

}

CNetworkListener::~CNetworkListener()
{

}

CIOCPSession* CNetworkListener::CreateIOCPSession()
{
	CServerSession* pNewSession = new CServerSession;
	return static_cast<CIOCPSession*>(pNewSession);
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)