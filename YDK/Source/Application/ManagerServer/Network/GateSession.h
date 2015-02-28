#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_GATESESSION_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_GATESESSION_H__INCLUDED

#include "Network/Session/ServerSession.h"

using Core::Utility::CServerSession;
using Core::Utility::uint32;

class CGateSession : public CServerSession
{
public:
	CGateSession();
	virtual ~CGateSession();

	uint32 GetGateID() const { return m_unGateID; }
	void SetGateID(uint32 val) { m_unGateID = val; }

protected:
	uint32 m_unGateID;
	
};

#endif