#include "ChatManager/ChatManager.h"
#include "Singleton.h"
#include "CharactorManager/CharactorManager.h"
#include "Network/GateServerListener.h"
#include "GroupManager/GroupManager.h"

using Core::Utility::TSingleton;

CChatManager::~CChatManager()
{

}

CChatManager::CChatManager()
{

}

void CChatManager::ProcessMsg( SMsg* pMsg )
{
	if (pMsg->GetType() == EWorldMsg_Chat)	
	{									
		CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg)); 
		SChatData chatData;										
		cPacket << chatData;

		DispatchMsg(chatData, pMsg->GetPlayerID());
	}
}

void CChatManager::OnDispatchSingleMessage( SChatData& chatData, uint32 unSenderID )
{
	CGamePlayer* pTargetPlayer = TSingleton<CCharactorManager>::GetInstance().GetPlayer(chatData.m_unTargetID);
	if (pTargetPlayer == NULL)
	{
		return;
	}

	CGamePlayer* pSender = TSingleton<CCharactorManager>::GetInstance().GetPlayer(unSenderID);
	if (pSender == NULL)
	{
		return;
	}

	//if send is receiver's friend
	SRelation& rRelation = pTargetPlayer->GetRelation();
	SRelationData* pRelationData = rRelation.FindRelation(pSender->GetPlayerID());
	if (pRelationData == NULL)
	{
		return;
	}

	chatData.m_strSenderName = pSender->GetNickName();
	CDynamicPacket cPacket(EWorldMsg_Chat);
	cPacket << chatData;
	cPacket.GetMsg()->SetPlayerID(pTargetPlayer->GetPlayerID());
	TSingleton<CGateServerListener>::GetInstance().SendMsg(cPacket.GetMsg(), pTargetPlayer->GetGateID());

}

void CChatManager::OnDispatchGroupMessage( SChatData& chatData, uint32 unSenderID )
{
	CGroup* pTargetGroup = TSingleton<CGroupManager>::GetInstance().GetGroup(chatData.m_unTargetID);
	if (pTargetGroup == NULL)
	{
		return;
	}

	CGamePlayer* pSender = TSingleton<CCharactorManager>::GetInstance().GetPlayer(unSenderID);
	if (pSender == NULL)
	{
		return;
	}

	chatData.m_strSenderName = pSender->GetNickName();
	CDynamicPacket cPacket(EWorldMsg_Chat);
	cPacket << chatData;
	pTargetGroup->SendMsgToAllMembers(cPacket.GetMsg());
}