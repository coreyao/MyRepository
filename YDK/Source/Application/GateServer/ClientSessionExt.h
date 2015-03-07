#ifndef CORE_UTILITY_APPLICATION_GATESERVER_CLIENTSESSIONEXT_H__INCLUDED
#define CORE_UTILITY_APPLICATION_GATESERVER_CLIENTSESSIONEXT_H__INCLUDED

#include "Network/Session/ServerSession.h"

using Core::Utility::CServerSession;
using Core::Utility::uint16;
using Core::Utility::uint32;

class CClientSessionExt : public CServerSession
{
public:
	CClientSessionExt();
	virtual ~CClientSessionExt();

	void AddRecvFlag();
	void ClearRecvFlag();
	uint16 GetRecvFlag();

	uint32 GetChallengeID();
	void SetChallengeID(uint32 val);

	uint32 GetAccountID() const { return m_unAccountID; }
	void SetAccountID(uint32 val) { m_unAccountID = val; }

protected:
	uint32 m_unAccountID;
	uint16 m_recvFlag;
	uint32 m_unChallengeID;
	bool m_bChecked;
};


#endif