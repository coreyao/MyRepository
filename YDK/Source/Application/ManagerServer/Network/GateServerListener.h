#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_GATESERVERLISTENER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_GATESERVERLISTENER_H__INCLUDED

#include "Network/Worker/NetworkListener.h"
#include "Lockers/Locker.h"
#include "GateSession.h"

using Core::Utility::CNetworkListener;
using Core::Utility::CIOCPSession;
using Core::Utility::CLocker;
using Core::Utility::uint32;
using Core::Utility::uint8;

class CGateServerListener : public CNetworkListener
{
public:
	CGateServerListener();
	virtual ~CGateServerListener();

	virtual void ProcessMsg();
	virtual void OnAcceptNewSession(CIOCPSession* pIOCPSession);
	virtual CIOCPSession* CreateIOCPSession();
	virtual void SendMsg(SMsg* msg, uint32 gateID = 0);

protected:
	std::vector<CGateSession*> m_gateSessions;
	CLocker m_lock;
};


#endif