#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_RELATIONMANAGER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_RELATIONMANAGER_H__INCLUDED

#include "CharactorManager/GamePlayer.h"
#include "MsgDef.h"

class CRelationManager
{
public:
	CRelationManager();
	~CRelationManager();

	BEGIN_MSG_PROCESS
		REGISTER_FUNC(ERelationMsg_ReqAddRelation, OnAddRelation, MsgReqAddRelation)
		REGISTER_FUNC(ERelationMsg_ReqDelRelation, OnDelRelation, MsgReqDelRelation)
		REGISTER_FUNC(ERelationMsg_ReqCreateCustomGroup, OnCreateCustomGroup, MsgReqCreateFriendGroup)
		REGISTER_FUNC(ERelationMsg_ReqChangeCustomGroup, OnChangeCustomGroup, MsgReqChangeCustomGroup)
		REGISTER_FUNC(ERelationMsg_ReqDelCustomGroup, OnDelCustomGroup, MsgReqDelCustomGroup)
	END_MSG_PROCESS

	void OnPlayerEnterWorld(CGamePlayer& rPlayer);
	void OnPlayerLeaveWorld(CGamePlayer& rPlayer);

	void UpdateSingleRelation(CGamePlayer& rPlayer, SRelationData& relationData, uint8 oper = SRelation::EOPER_AddRelation);
	void UpdateAllRelation(CGamePlayer& rPlayer);
	void SaveAllRelation(CGamePlayer& rPlayer);

	void UpdateSingleCustomGroup(CGamePlayer& rPlayer, SFriendGroupData& friendGroupData, uint8 oper = SRelation::EOPER_AddFriendGroup);

protected:
	void OnAddRelation(MsgReqAddRelation* pMsg);
	void OnDelRelation(MsgReqDelRelation* pMsg);
	void OnCreateCustomGroup(MsgReqCreateFriendGroup* pMsg);
	void OnChangeCustomGroup(MsgReqChangeCustomGroup* pMsg);
	void OnDelCustomGroup(MsgReqDelCustomGroup* pMsg);

private:
	void _OnProcessRelation(uint32 unSenderID, const XString& strTargetAccount, uint8 oper);
	void _OnProcessActivity(CGamePlayer& rPlayer, bool bOnline);
	uint32 _GenerateFriendGroupID(CGamePlayer& rPlayer);

	uint32 m_unLastFriendGroupID;
};

#endif