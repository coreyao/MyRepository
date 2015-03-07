#ifndef CORE_UTILITY_APPLICATION_DBSERVER_MANAGERSERVERLISTENER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_DBSERVER_MANAGERSERVERLISTENER_H__INCLUDED

#include "Network/Worker/NetworkListener.h"
#include "Lockers/Locker.h"
#include "Network/Session/ServerSession.h"

using Core::Utility::CNetworkListener;
using Core::Utility::CIOCPSession;
using Core::Utility::uint32;
using Core::Utility::uint8;
using Core::Utility::CLocker;
using Core::Utility::CServerSession;

class CManagerServerListener : public CNetworkListener
{
public:
	CManagerServerListener();
	virtual ~CManagerServerListener();

	virtual void ProcessMsg();
	virtual void OnAcceptNewSession(CIOCPSession* pIOCPSession);
	virtual CIOCPSession* CreateIOCPSession();

private:
	CLocker m_lock;
	std::vector<CServerSession*> m_managerSessions;
};


#endif