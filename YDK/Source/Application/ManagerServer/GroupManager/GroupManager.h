#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_GROUPMANAGER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_GROUPMANAGER_H__INCLUDED

#include "CharactorManager/GamePlayer.h"
#include "Network/Packet/DynamicPacket.h"
#include "MsgDef.h"
#include "CommonDataStruct/GroupData_Struct.h"
#include "GroupManager/Group.h"
#include "GameObject.h"

using Core::Utility::SMsgDynamic;

class CGroupManager : public CGameObject
{
public:
	typedef std::map<uint32, CGroup*> MAPGROUPS;

public:
	CGroupManager();
	~CGroupManager();

	void Init();
	void OnTimer(uint32 unTimerID);

	BEGIN_MSG_PROCESS
		REGISTER_FUNC(EGroupMsg_D2M_AckLoadGroup, OnLoadGroup, SMsgDynamic)
		REGISTER_FUNC(EGroupMsg_C2S_ReqCreate, OnPrepareCreateGroup, MsgReqCreateGroup)
		REGISTER_FUNC(EGroupMsg_D2M_AckCreate, OnCreateGroup, SMsgDynamic)
		REGISTER_FUNC(EGroupMsg_C2S_ReqJoin, OnJoinGroup, MsgReqJoinGroup)
		REGISTER_FUNC(EGroupMsg_C2S_ReqLeave, OnLeaveGroup, MsgReqLeaveGroup)
	END_MSG_PROCESS

	CGroup* GetGroup(uint32 unGroupID);
	void OnMemberEnterWorld(CGamePlayer& rPlayer);
	void OnMemberLeaveWorld(CGamePlayer& rPlayer);

	void UpdateMemberAllGroups(CGamePlayer& rPlayer);
	void SaveMemberAllGroups(CGamePlayer& rPlayer);

protected:
	void OnLoadGroup(SMsgDynamic* pMsg);
	void OnPrepareCreateGroup(MsgReqCreateGroup* pMsg);
	void OnCreateGroup(SMsgDynamic* pMsg);
	void OnJoinGroup(MsgReqJoinGroup* pMsg);
	void OnLeaveGroup(MsgReqLeaveGroup* pMsg);

protected:
	void _OnProcessMemberActivity(CGamePlayer& rPlayer, bool bOnline);

protected:
	MAPGROUPS m_mapGroups;
	bool m_bInited;
};

#endif