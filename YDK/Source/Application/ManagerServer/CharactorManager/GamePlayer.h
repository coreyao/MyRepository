#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_GAMEPLAYER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_GAMEPLAYER_H__INCLUDED

#include "XString/XString.h"
#include "CommonDataStruct/PlayerInfo_Struct.h"
#include "CommonDataStruct/GroupData_Struct.h"

using Core::Utility::XString;
using Core::Utility::uint32;

class CGamePlayer
{
public:
	typedef std::set<uint32> SETPLAYERGROUPS;

public:
	CGamePlayer();
	virtual ~CGamePlayer();

	uint32 GetPlayerID() const { return m_unPlayerID; }
	void SetPlayerID(uint32 val) { m_unPlayerID = val; }

	uint32 GetGateID() const { return m_unGateID; }
	void SetGateID(uint32 val) { m_unGateID = val; }

	XString GetNickName();
	void SetNickName(const XString& nickName);

	SRelation& GetRelation(){return m_playerRelation;}

	SETPLAYERGROUPS& GetGroups() {return m_playerGroups;}
	void SetGroups(const SETPLAYERGROUPS& groups) {m_playerGroups = groups;}

protected:
	uint32 m_unPlayerID;
	uint32 m_unGateID;
	SPlayerBaseInfo m_baseInfo;
	SRelation m_playerRelation;
	SETPLAYERGROUPS m_playerGroups;
};

#endif
