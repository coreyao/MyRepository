#ifndef CORE_UTILITY_APPLICATION_GATESERVER_GATECLIENT_H__INCLUDED
#define CORE_UTILITY_APPLICATION_GATESERVER_GATECLIENT_H__INCLUDED

#include "ClientSessionExt.h"

class CGateClient
{
public:
	CGateClient();
	virtual ~CGateClient();

	uint32 GetAccountID() const { return m_unAccountID; }
	void SetAccountID(uint32 val) { m_unAccountID = val; }

	CClientSessionExt* GetClientSession() const { return m_clientSession; }
	void SetClientSession(CClientSessionExt* val) { m_clientSession = val; }

	bool IsInWorld() const { return m_bIsInWorld; }
	void SetInWorld(bool val) { m_bIsInWorld = val; }

	SMsg* PopMsgPack();

private:
	uint32 m_unAccountID;
	CClientSessionExt* m_clientSession;
	bool m_bIsInWorld;
	
};

#endif