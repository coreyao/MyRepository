#include "GroupManager/Group.h"
#include "MsgDef.h"
#include "CharactorManager/CharactorManager.h"
#include "Singleton.h"
#include "Network/GateServerListener.h"
#include "Network/DBServerConnector.h"

using Core::Utility::TSingleton;


CGroup::CGroup()
{
}

CGroup::~CGroup()
{
}

void CGroup::UpdateGroupMember( SGroupMemeber& groupMember, uint8 oper /*= SGroupData::EOPER_JoinGroup*/ )
{
	CDynamicPacket cPacket(EPlayerMsg_S2C_UpdateGroupMember);
	cPacket << oper << m_unGroupID;
	groupMember.SerializeClient(cPacket);
	SendMsgToAllMembers(cPacket.GetMsg());
}

void CGroup::UpdateGroup(uint32 memberID, uint8 oper /*= SGroups::EOPER_AddGroup*/ )
{
	CDynamicPacket cPacket(EPlayerMsg_S2C_UpdateGroup);
	cPacket << oper;
	SerializeClient(cPacket);

	if (memberID != 0)
	{
		CGamePlayer* pPlayer = TSingleton<CCharactorManager>::GetInstance().GetPlayer(memberID);
		if (pPlayer != NULL)
		{
			cPacket.GetMsg()->SetPlayerID(pPlayer->GetPlayerID());
			TSingleton<CGateServerListener>::GetInstance().SendMsg(cPacket.GetMsg(), pPlayer->GetGateID());
		}
	}
	else
	{
		SendMsgToAllMembers(cPacket.GetMsg());
	}
}

void CGroup::SaveGroup()
{
	CDynamicPacket cPacket(EGroupMsg_M2D_SaveGroupsInfo);
	cPacket << *this;
	TSingleton<CDBServerConnector>::GetInstance().SendMsg(cPacket.GetMsg());
}

void CGroup::SendMsgToAllMembers( SMsg* pMsg )
{
	MAPGROUPMEMBERS::iterator memberIter = m_mapMembers.begin();
	MAPGROUPMEMBERS::iterator memberIterEnd = m_mapMembers.end();
	for (; memberIter != memberIterEnd; ++memberIter)
	{
		CGamePlayer* pMember = TSingleton<CCharactorManager>::GetInstance().GetPlayer(memberIter->second.m_unMemeberID);
		if (pMember != NULL)
		{
			pMsg->SetPlayerID(pMember->GetPlayerID());
			TSingleton<CGateServerListener>::GetInstance().SendMsg(pMsg, pMember->GetGateID());
		}
	}
}
