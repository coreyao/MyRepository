#include "CharactorManager/CharactorManager.h"
#include "LogSystem.h"
#include "AccountManager/AccountManager.h"
#include "Singleton.h"
#include "Network/GateServerListener.h"
#include "RelationManager/RelationManager.h"
#include "GroupManager/GroupManager.h"

using Core::Utility::TSingleton;

CCharactorManager::CCharactorManager()
{
}

CCharactorManager::~CCharactorManager()
{
}

void CCharactorManager::OnPlayerEnterWorld(DBMsgAckEnterWorld* dbAckEnterWorldMsg)
{
	if (dbAckEnterWorldMsg == NULL)
	{
		return;
	}

	SAccount* pAccount = TSingleton<CAccountManager>::GetInstance().GetAccount(dbAckEnterWorldMsg->GetPlayerID());
	if (pAccount == NULL)
	{
		return;
	}

	MsgAckEnterWorld ackMsg;
	ackMsg.SetPlayerID(dbAckEnterWorldMsg->GetPlayerID());
	ackMsg.m_result = dbAckEnterWorldMsg->m_result;
	ackMsg.m_unChallengeID = pAccount->m_unChallengeID;

	if (dbAckEnterWorldMsg->m_result == DBMsgAckEnterWorld::EResult_Success)
	{
		CGamePlayer* newPlayer = new CGamePlayer;
		newPlayer->SetPlayerID(dbAckEnterWorldMsg->GetPlayerID());
		newPlayer->SetNickName(dbAckEnterWorldMsg->m_baseInfo.m_nickName);
		newPlayer->SetGateID(pAccount->m_unGateID);

		//relation data
		CDynamicPacket cRelationPacket(dbAckEnterWorldMsg->m_friendsInfo, dbAckEnterWorldMsg->m_friendsInfoLen);
		newPlayer->GetRelation().Serialize(cRelationPacket);
		TSingleton<CRelationManager>::GetInstance().OnPlayerEnterWorld(*newPlayer);

		//group data
		CDynamicPacket cGroupPacket(dbAckEnterWorldMsg->m_groupsInfo, dbAckEnterWorldMsg->m_groupsInfoLen);
		CGamePlayer::SETPLAYERGROUPS tempGroups;
		cGroupPacket << tempGroups;
		newPlayer->SetGroups(tempGroups);
		TSingleton<CGroupManager>::GetInstance().OnMemberEnterWorld(*newPlayer);

		m_mapPlayers.insert(std::make_pair(newPlayer->GetPlayerID(), newPlayer));
		Core::LOG(XString().Format("Player %s Enter World", newPlayer->GetNickName().c_str()).c_str());

		//sync client
		strcpy_s(ackMsg.m_playerBaseInfo.m_nickName, newPlayer->GetNickName().c_str());
		TSingleton<CGateServerListener>::GetInstance().SendMsg(&ackMsg, pAccount->m_unGateID);
		TSingleton<CRelationManager>::GetInstance().UpdateAllRelation(*newPlayer);
		TSingleton<CGroupManager>::GetInstance().UpdateMemberAllGroups(*newPlayer);
	}
	else
	{
		TSingleton<CGateServerListener>::GetInstance().SendMsg(&ackMsg, pAccount->m_unGateID);
	}
}

void CCharactorManager::OnPlayerLeaveWorld( uint32 unPlayerID )
{
	MAPPLAYERS::iterator iter = m_mapPlayers.find(unPlayerID);
	if (iter != m_mapPlayers.end())
	{
		TSingleton<CAccountManager>::GetInstance().OnLeave(unPlayerID);
		TSingleton<CRelationManager>::GetInstance().OnPlayerLeaveWorld(*iter->second);
		TSingleton<CGroupManager>::GetInstance().OnMemberLeaveWorld(*iter->second);

		XString nickName = iter->second->GetNickName();
		m_mapPlayers.erase(iter);
		Core::LOG(XString().Format("Player %s Leave World", nickName.c_str()).c_str());
	}
}

CGamePlayer* CCharactorManager::GetPlayer( uint32 playerID )
{
	MAPPLAYERS::iterator iter = m_mapPlayers.find(playerID);
	if (iter != m_mapPlayers.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}
