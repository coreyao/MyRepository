#ifndef CORE_UTILITY_APPLICATION_GATESERVER_CLIENTLISTENER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_GATESERVER_CLIENTLISTENER_H__INCLUDED

#include "Network/Worker/NetworkListener.h"

using Core::Utility::CNetworkListener;
using Core::Utility::CIOCPSession;
using Core::Utility::uint32;

class CClientListener : public CNetworkListener
{
public:
	CClientListener();
	virtual ~CClientListener();

	virtual void ProcessMsg();
	virtual void OnAcceptNewSession(CIOCPSession* pIOCPSession);
	virtual CIOCPSession* CreateIOCPSession();

	uint32 GetOnlineClientsCount();

};


#endif