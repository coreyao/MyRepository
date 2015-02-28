#include "RelationManager/RelationManager.h"
#include "CharactorManager/CharactorManager.h"
#include "AccountManager/AccountManager.h"
#include "Singleton.h"
#include "Network/GateServerListener.h"
#include "Network/DBServerConnector.h"

using Core::Utility::TSingleton;

CRelationManager::CRelationManager()
: m_unLastFriendGroupID(0)
{

}

CRelationManager::~CRelationManager()
{

}

void CRelationManager::OnPlayerEnterWorld(CGamePlayer& rPlayer)
{
	//add default friend group
	const uint32 conDefaultFriendGroupID = 0;
	if (NULL == rPlayer.GetRelation().FindFriendGroup(conDefaultFriendGroupID))
	{
		SFriendGroupData friendGroupData;
		friendGroupData.m_unFriendGroupID = 0;
		friendGroupData.m_strFriendGroupName = TEXT("ÎÒµÄºÃÓÑ");
		rPlayer.GetRelation().AddFriendGroup(friendGroupData);
	}
	
	_OnProcessActivity(rPlayer, true);
}

void CRelationManager::OnPlayerLeaveWorld( CGamePlayer& rPlayer )
{
	_OnProcessActivity(rPlayer, false);
}

void CRelationManager::UpdateSingleRelation( CGamePlayer& rPlayer, SRelationData& relationData, uint8 oper /*= SRelation::EOPER_AddRelation*/ )
{
	if (oper == SRelation::EOPER_AddRelation)
	{
		rPlayer.GetRelation().AddRelation(relationData);
	}
	else if (oper == SRelation::EOPER_DelRelation)
	{
		rPlayer.GetRelation().DelRelation(relationData);
	}

	CDynamicPacket cPacket(EPlayerMsg_M2C_UpdateRelationData);
	cPacket.GetMsg()->SetPlayerID(rPlayer.GetPlayerID());
	cPacket << oper;
	relationData.SerializeClient(cPacket);
	TSingleton<CGateServerListener>::GetInstance().SendMsg(cPacket.GetMsg(), rPlayer.GetGateID());
}

void CRelationManager::UpdateAllRelation( CGamePlayer& rPlayer )
{
	CDynamicPacket cPacket(EPlayerMsg_M2C_UpdateAllRelationData);
	cPacket.GetMsg()->SetPlayerID(rPlayer.GetPlayerID());
	rPlayer.GetRelation().SerializeClient(cPacket);
	TSingleton<CGateServerListener>::GetInstance().SendMsg(cPacket.GetMsg(), rPlayer.GetGateID());
}

void CRelationManager::SaveAllRelation( CGamePlayer& rPlayer )
{
	CDynamicPacket cPacket(EPlayerMsg_M2D_SaveRelationData);
	cPacket.GetMsg()->SetPlayerID(rPlayer.GetPlayerID());
	cPacket << rPlayer.GetRelation();
	TSingleton<CDBServerConnector>::GetInstance().SendMsg(cPacket.GetMsg());
}

void CRelationManager::OnAddRelation(MsgReqAddRelation* pMsg)
{
	_OnProcessRelation(pMsg->GetPlayerID(), pMsg->m_targetAccount, SRelation::EOPER_AddRelation);
}

void CRelationManager::OnDelRelation(MsgReqDelRelation* pMsg)
{
	SAccount* pTargetAccount = TSingleton<CAccountManager>::GetInstance().GetAccount(pMsg->m_unPlayerID);
	if (pTargetAccount == NULL)
	{
		return;
	}

	_OnProcessRelation(pMsg->GetPlayerID(), pTargetAccount->m_account, SRelation::EOPER_DelRelation);
}

void CRelationManager::_OnProcessRelation(uint32 unSenderID, const XString& strTargetAccount, uint8 oper)
{
	CGamePlayer* pSender = TSingleton<CCharactorManager>::GetInstance().GetPlayer(unSenderID);
	if (pSender == NULL)
	{
		return;
	}

	SAccount* pTargetAccount = TSingleton<CAccountManager>::GetInstance().GetAccount(strTargetAccount.c_str());
	if (pTargetAccount == NULL)
	{
		return;
	}

	CGamePlayer* pTarget = TSingleton<CCharactorManager>::GetInstance().GetPlayer(pTargetAccount->m_unAccountID);
	if (pTarget == NULL)
	{
		return;
	}

	SRelationData senderData;
	senderData.m_unCharactorID = pSender->GetPlayerID();
	senderData.m_strNickname = pSender->GetNickName();
	senderData.m_bIsOnline = true;
	UpdateSingleRelation(*pTarget, senderData, oper == SRelation::EOPER_DelRelation ? oper : SRelation::EOPER_AddRelation);
	SaveAllRelation(*pTarget);

	SRelationData targetData;
	targetData.m_unCharactorID = pTarget->GetPlayerID();
	targetData.m_strNickname = pTarget->GetNickName();
	targetData.m_bIsOnline = true;
	UpdateSingleRelation(*pSender, targetData, oper == SRelation::EOPER_DelRelation ? oper : SRelation::EOPER_AddRelation);
	SaveAllRelation(*pSender);
}

void CRelationManager::_OnProcessActivity( CGamePlayer& rPlayer, bool bOnline)
{
	std::map<uint32, SRelationData>::iterator relationIter = rPlayer.GetRelation().m_mapRelations.begin();
	std::map<uint32, SRelationData>::iterator relationIterEnd = rPlayer.GetRelation().m_mapRelations.end();
	for (; relationIter != relationIterEnd; ++relationIter)
	{
		CGamePlayer* pFriend = TSingleton<CCharactorManager>::GetInstance().GetPlayer(relationIter->second.m_unCharactorID);
		if (pFriend != NULL)
		{
			relationIter->second.m_bIsOnline = true;
			SRelation& rRelation = pFriend->GetRelation();
			SRelationData* pRelationData = rRelation.FindRelation(rPlayer.GetPlayerID());
			if (pRelationData != NULL)
			{
				pRelationData->m_bIsOnline = bOnline;
				UpdateSingleRelation(*pFriend, *pRelationData, SRelation::EOPER_AddRelation);
			}
		}
		else
		{
			relationIter->second.m_bIsOnline = false;
		}
	}
}

void CRelationManager::OnCreateCustomGroup( MsgReqCreateFriendGroup* pMsg )
{
	CGamePlayer* pSender = TSingleton<CCharactorManager>::GetInstance().GetPlayer(pMsg->GetPlayerID());
	if (pSender == NULL)
	{
		return;
	}

	SFriendGroupData friendGroupData;
	friendGroupData.m_unFriendGroupID = _GenerateFriendGroupID(*pSender);
	friendGroupData.m_strFriendGroupName = pMsg->m_groupName;
	UpdateSingleCustomGroup(*pSender, friendGroupData);
	SaveAllRelation(*pSender);
}

void CRelationManager::OnChangeCustomGroup( MsgReqChangeCustomGroup* pMsg )
{
	CGamePlayer* pSender = TSingleton<CCharactorManager>::GetInstance().GetPlayer(pMsg->GetPlayerID());
	if (pSender == NULL)
	{
		return;
	}

	SRelation& rRelation = pSender->GetRelation();
	SRelationData* pRelationData = rRelation.FindRelation(pMsg->m_unFriendID);
	if (pRelationData != NULL)
	{
		if (pRelationData->m_unFriendGroupID != pMsg->m_unNewCustomGroupID)
		{
			pRelationData->m_unFriendGroupID = pMsg->m_unNewCustomGroupID;
			UpdateSingleRelation(*pSender, *pRelationData);
			SaveAllRelation(*pSender);
		}
	}
}

void CRelationManager::OnDelCustomGroup( MsgReqDelCustomGroup* pMsg )
{
	CGamePlayer* pSender = TSingleton<CCharactorManager>::GetInstance().GetPlayer(pMsg->GetPlayerID());
	if (pSender == NULL)
	{
		return;
	}

	SRelation& rRelation = pSender->GetRelation();
	std::vector<uint32> playersInFriendGroup = rRelation.GetPlayersInFriendGroup(pMsg->m_unCustomGroupID);
	if (playersInFriendGroup.empty())
	{
		SFriendGroupData tempFriendGroupData;
		tempFriendGroupData.m_unFriendGroupID = pMsg->m_unCustomGroupID;
		UpdateSingleCustomGroup(*pSender, tempFriendGroupData, SRelation::EOPER_DelFriendGroup);
		SaveAllRelation(*pSender);
	}
}

uint32 CRelationManager::_GenerateFriendGroupID(CGamePlayer& rPlayer)
{
	SRelation& rRelation = rPlayer.GetRelation();
	do 
	{
		++m_unLastFriendGroupID;
	} while (rRelation.FindFriendGroup(m_unLastFriendGroupID) != NULL);
	
	return m_unLastFriendGroupID;
}

void CRelationManager::UpdateSingleCustomGroup( CGamePlayer& rPlayer, SFriendGroupData& friendGroupData, uint8 oper /*= SRelation::EOPER_AddFriendGroup*/ )
{
	if (oper == SRelation::EOPER_AddFriendGroup)
	{
		rPlayer.GetRelation().AddFriendGroup(friendGroupData);
	}
	else if (oper == SRelation::EOPER_DelFriendGroup)
	{
		rPlayer.GetRelation().DelFriendGroup(friendGroupData);
	}

	CDynamicPacket cPacket(EPlayerMsg_M2C_UpdateFriendGroupData);
	cPacket.GetMsg()->SetPlayerID(rPlayer.GetPlayerID());
	cPacket << oper;
	cPacket << friendGroupData;
	TSingleton<CGateServerListener>::GetInstance().SendMsg(cPacket.GetMsg(), rPlayer.GetGateID());
}
