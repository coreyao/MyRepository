#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_CLIENTCLIENT_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_CLIENTCLIENT_H__INCLUDED

#include "CommonDataStruct/PlayerInfo_Struct.h"
#include "CommonDataStruct/GroupData_Struct.h"

#define INVALID_PLAYER_ID 0xffffffff
#define INVALID_GROUP_ID 0xffffffff

class CClientPlayer
{
public:
	CClientPlayer();
	virtual ~CClientPlayer();

	XString GetNickName();
	void SetNickName(const XString& nickName);

	uint32 GetAccountID();
	void SetAccountID(uint32 accountID);

	SRelation& GetRelation() {return m_playerRelation;}
	void SetRelation(SRelation& relation) {m_playerRelation = relation;}

	SGroups& GetGroups() {return m_playerGroup;}
	void SetGroups(SGroups& groups) {m_playerGroup = groups;}

	XString GetFriendName(uint32 unPlayerID);
	XString GetGroupName(uint32 unGroupID);
	XString GetGroupMemberName(uint32 unGroupID, uint32 unGroupMemberID);

private:
	uint32 m_accountID;
	SPlayerBaseInfo m_playerBaseInfo;
	SRelation m_playerRelation;
	SGroups m_playerGroup;
};


#endif