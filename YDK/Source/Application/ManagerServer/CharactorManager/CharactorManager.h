#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_CHARACTORMANAGER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_CHARACTORMANAGER_H__INCLUDED

#include <map>
#include "CharactorManager/GamePlayer.h"
#include "Network/Packet/DynamicPacket.h"
#include "MsgDef.h"

using Core::Utility::SMsgDynamic;

class CCharactorManager
{
public:
	typedef std::map<uint32, CGamePlayer*> MAPPLAYERS;

public:
	CCharactorManager();
	~CCharactorManager();

	//BEGIN_MSG_PROCESS
	//END_MSG_PROCESS

	void OnPlayerEnterWorld(DBMsgAckEnterWorld* dbAckEnterWorldMsg);
	void OnPlayerLeaveWorld(uint32 unPlayerID);
	CGamePlayer* GetPlayer(uint32 playerID);

protected:
	MAPPLAYERS m_mapPlayers;
};

#endif