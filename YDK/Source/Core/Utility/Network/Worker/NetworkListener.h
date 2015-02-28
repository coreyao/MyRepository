#ifndef CORE_UTILITY_NETWORK_WORKER_NETWORKLISTENER_H__INCLUDED
#define CORE_UTILITY_NETWORK_WORKER_NETWORKLISTENER_H__INCLUDED

#include "Network/tcp/TcpServer.h"
#include "Network/Session/IOCPSession.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CNetworkListener : public CTcpServer
{
public:
	CNetworkListener();
	virtual ~CNetworkListener();

	virtual void ProcessMsg() = 0;

protected:
	virtual CIOCPSession* CreateIOCPSession();
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif
