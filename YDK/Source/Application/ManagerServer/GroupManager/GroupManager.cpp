#include "GroupManager/GroupManager.h"
#include "Singleton.h"
#include "Network/DBServerConnector.h"
#include "CharactorManager/CharactorManager.h"
#include "LogSystem.h"
#include "Network/GateServerListener.h"

using Core::Utility::TSingleton;

CGroupManager::CGroupManager()
	: m_bInited(false)
{
}

CGroupManager::~CGroupManager()
{
	MAPGROUPS::iterator iter = m_mapGroups.begin();
	for (; iter != m_mapGroups.end(); ++iter)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
}

void CGroupManager::Init()
{
	SetTimer(GROUP_LOAD_TIMER, GROUP_LOAD_TIMER_INTERVAL);
}

void CGroupManager::OnTimer( uint32 unTimerID )
{
	switch (unTimerID)
	{
	case GROUP_LOAD_TIMER:
		{
			if (m_bInited)
			{
				StopTimer(GROUP_LOAD_TIMER);
				return;
			}

			CDynamicPacket cPacket(EGroupMsg_M2D_ReqLoadGroup);
			TSingleton<CDBServerConnector>::GetInstance().SendMsg(cPacket.GetMsg());
		}
		break;
	}
}

CGroup* CGroupManager::GetGroup( uint32 unGroupID )
{
	MAPGROUPS::iterator iter = m_mapGroups.find(unGroupID);
	if (iter != m_mapGroups.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

void CGroupManager::OnLoadGroup( SMsgDynamic* pMsg )
{
	if (m_bInited)
	{
		return;
	}

	std::vector<CGroup> vecGroups;
	CDynamicPacket cPacket(pMsg);
	cPacket << vecGroups;

	std::vector<CGroup>::iterator iter = vecGroups.begin();
	for (; iter != vecGroups.end(); ++iter)
	{
		CGroup* newGroup = new CGroup(*iter);
		m_mapGroups.insert(std::make_pair(newGroup->m_unGroupID, newGroup));
		Core::LOG(XString().Format(TEXT("Group:%d:%s"), newGroup->m_unGroupID, newGroup->m_strGroupName.c_str()).c_str());
	}

	Core::LOG(TEXT("Groups Loaded!"));

	m_bInited = true;
}

void CGroupManager::OnMemberEnterWorld( CGamePlayer& rPlayer )
{
	_OnProcessMemberActivity(rPlayer, true);
}

void CGroupManager::OnMemberLeaveWorld( CGamePlayer& rPlayer )
{
	_OnProcessMemberActivity(rPlayer, false);
}

void CGroupManager::_OnProcessMemberActivity( CGamePlayer& rPlayer, bool bOnline )
{
	SGroupMemeber memberSelf;
	memberSelf.m_unMemeberID = rPlayer.GetPlayerID();
	memberSelf.m_strName = rPlayer.GetNickName();
	memberSelf.m_bIsOnline = bOnline;

	CGamePlayer::SETPLAYERGROUPS::iterator groupIter = rPlayer.GetGroups().begin();
	CGamePlayer::SETPLAYERGROUPS::iterator groupIterEnd = rPlayer.GetGroups().end();
	for (; groupIter != groupIterEnd; ++groupIter)
	{
		CGroup* pGroup = GetGroup(*groupIter);
		if (pGroup == NULL)
		{
			continue;
		}

		pGroup->JoinGroup(memberSelf);
		pGroup->UpdateGroupMember(memberSelf);
	}
}

void CGroupManager::OnPrepareCreateGroup(MsgReqCreateGroup* pMsg)
{
	TSingleton<CDBServerConnector>::GetInstance().SendMsg(pMsg);
}

void CGroupManager::OnCreateGroup(SMsgDynamic* pMsg)
{
	CGroup* pGroup = new CGroup;
	CDynamicPacket cPacket(pMsg);
	cPacket << *pGroup;
	m_mapGroups.insert(std::make_pair(pGroup->m_unGroupID, pGroup));
	Core::LOG(XString().Format(TEXT("Group %s Created"), pGroup->m_strGroupName.c_str()).c_str());

	CGamePlayer* pPlayer = TSingleton<CCharactorManager>::GetInstance().GetPlayer(pMsg->GetPlayerID());
	if (pPlayer != NULL)
	{
		SGroupMemeber memberSelf;
		memberSelf.m_unMemeberID = pPlayer->GetPlayerID();
		memberSelf.m_strName = pPlayer->GetNickName();
		memberSelf.m_bIsOnline = true;
		pGroup->JoinGroup(memberSelf);
		pGroup->SaveGroup();

		pPlayer->GetGroups().insert(pGroup->m_unGroupID);
		SaveMemberAllGroups(*pPlayer);

		pGroup->UpdateGroup();
	}
}

void CGroupManager::OnJoinGroup( MsgReqJoinGroup* pMsg )
{
	CGamePlayer* pPlayer = TSingleton<CCharactorManager>::GetInstance().GetPlayer(pMsg->GetPlayerID());
	if (pPlayer == NULL)
	{
		return;
	}

	CGroup* pGroup = GetGroup(pMsg->m_unGroupID);
	if (pGroup == NULL)
	{
		return;
	}

	SGroupMemeber groupMember;
	groupMember.m_unMemeberID = pPlayer->GetPlayerID();
	groupMember.m_strName = pPlayer->GetNickName();
	groupMember.m_bIsOnline = true;

	pGroup->JoinGroup(groupMember);
	pGroup->SaveGroup();

	pPlayer->GetGroups().insert(pGroup->m_unGroupID);
	SaveMemberAllGroups(*pPlayer);

	pGroup->UpdateGroup(pPlayer->GetPlayerID());
	pGroup->UpdateGroupMember(groupMember);
}

void CGroupManager::OnLeaveGroup( MsgReqLeaveGroup* pMsg )
{
	CGamePlayer* pPlayer = TSingleton<CCharactorManager>::GetInstance().GetPlayer(pMsg->GetPlayerID());
	if (pPlayer == NULL)
	{
		return;
	}

	CGroup* pGroup = GetGroup(pMsg->m_unGroupID);
	if (pGroup == NULL)
	{
		return;
	}

	SGroupMemeber groupMember;
	groupMember.m_unMemeberID = pPlayer->GetPlayerID();
	groupMember.m_strName = pPlayer->GetNickName();
	groupMember.m_bIsOnline = true;

	pGroup->LeaveGroup(groupMember);
	pGroup->SaveGroup();

	pPlayer->GetGroups().erase(pGroup->m_unGroupID);
	SaveMemberAllGroups(*pPlayer);
	
	pGroup->UpdateGroup(pPlayer->GetPlayerID(), SGroups::EOPER_DelGroup);
	pGroup->UpdateGroupMember(groupMember, SGroupData::EOPER_LeaveGroup);
}

void CGroupManager::UpdateMemberAllGroups( CGamePlayer& rPlayer )
{
	CDynamicPacket cPacket(EPlayerMsg_S2C_UpdateAllGroup);
	cPacket.GetMsg()->SetPlayerID(rPlayer.GetPlayerID());

	SGroups groups;
	CGamePlayer::SETPLAYERGROUPS::iterator iter = rPlayer.GetGroups().begin();
	CGamePlayer::SETPLAYERGROUPS::iterator iterEnd = rPlayer.GetGroups().end();
	for (; iter != iterEnd; ++iter)
	{
		CGroup* pGroup = TSingleton<CGroupManager>::GetInstance().GetGroup(*iter);
		if (pGroup != NULL)
		{
			groups.AddGroup(*pGroup);
		}
	}

	groups.SerializeClient(cPacket);
	TSingleton<CGateServerListener>::GetInstance().SendMsg(cPacket.GetMsg(), rPlayer.GetGateID());
}

void CGroupManager::SaveMemberAllGroups( CGamePlayer& rPlayer )
{
	CDynamicPacket cPacket(EPlayerMsg_M2D_SaveGroupData);
	cPacket.GetMsg()->SetPlayerID(rPlayer.GetPlayerID());
	cPacket << rPlayer.GetGroups();

	TSingleton<CDBServerConnector>::GetInstance().SendMsg(cPacket.GetMsg());
}
