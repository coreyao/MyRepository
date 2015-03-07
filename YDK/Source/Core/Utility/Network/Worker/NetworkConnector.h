#ifndef CORE_UTILITY_NETWORK_WORKER_NETWORKCONNECTOR_H__INCLUDED
#define CORE_UTILITY_NETWORK_WORKER_NETWORKCONNECTOR_H__INCLUDED

#include "Network/Session/ClientSession.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CNetworkConnector
{
public:
	CNetworkConnector();
	virtual ~CNetworkConnector();

	virtual void Init(const XString& strIP, uint16 uPort);
	virtual void CheckConnection();
	virtual void ProcessMsg() = 0;
	virtual bool SendMsg(SMsg* msg);

	virtual bool IsConnected();
	virtual bool IsConnecting();
	virtual void Close();

protected:
	XString m_strIP;
	uint16 m_uPort;
	CClientSession* m_pSession;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif