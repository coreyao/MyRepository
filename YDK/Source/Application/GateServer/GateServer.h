#ifndef CORE_UTILITY_APPLICATION_GATESERVER_GATESERVER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_GATESERVER_GATESERVER_H__INCLUDED

#include <map>
#include "Singleton.h"
#include "ClientSessionExt.h"
#include "GateClient.h"
#include "Lockers/Locker.h"
#include "Network/ThreadServer.h"
#include "MsgDef.h"
#include "XString/XString.h"
#include "Network/Packet/DynamicPacket.h"

using Core::Utility::CThreadServer;
using Core::Utility::TSingleton;
using Core::Utility::CLocker;
using Core::Utility::uint32;
using Core::Utility::uint8;
using Core::Utility::XString;
using Core::Utility::SMsgDynamic;

class CGateServer : public CThreadServer
{
public:
	typedef std::map<uint32, CClientSessionExt*> MAPCLIENTSEXT;
	typedef std::map<uint32, CGateClient*> MAPCLIENTS;

public:
	CGateServer();
	virtual ~CGateServer();

	void Init();
	virtual void Process();
	void OnAcceptNewSession(CClientSessionExt* newSession);
	void SyncSessions();
	void ProcessLoginClientMsg();
	void ProcessClientMsg();
	virtual void ProcessCommand();
	virtual void OnCommand(const XString& strCmd, const std::vector<XString>& params);

	uint8 GetGateID() const { return m_unGateID; }
	void SetGateID(uint8 val) { m_unGateID = val; }

	void SendMsgToClientByChallengeID(SMsg* msg, uint32 ChallengeID);

	void OnAckLoginCheck(MsgAckLoginCheck* ackMsg);
	void OnAckPlayerEnterWorld(MsgAckEnterWorld* pAckMsg);
	void OnDispatchMsgToClient(SMsg* pMsg);
	void OnAckRegisterAccount(MsgAckRegisterAccount* pAckMsg);

protected:
	MAPCLIENTSEXT m_tempSessions;
	MAPCLIENTSEXT m_loginSessions;

	MAPCLIENTS m_charactors;

	CLocker m_lock;
	uint32 m_unLastChallengeID;
	uint32 m_unLastSyncTime;
	uint8 m_unGateID;
	
};


#endif