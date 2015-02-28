#include "ClientPlayer.h"

CClientPlayer::CClientPlayer()
: m_accountID(0)
{
}

CClientPlayer::~CClientPlayer()
{
}

XString CClientPlayer::GetNickName()
{
	return m_playerBaseInfo.m_nickName;
}

void CClientPlayer::SetNickName( const XString& nickName )
{
	strcpy_s(m_playerBaseInfo.m_nickName, nickName.c_str());
}

uint32 CClientPlayer::GetAccountID()
{
	return m_accountID;
}

void CClientPlayer::SetAccountID( uint32 accountID )
{
	m_accountID = accountID;
}

XString CClientPlayer::GetFriendName( uint32 unPlayerID )
{
	SRelationData* pRelationData = m_playerRelation.FindRelation(unPlayerID);
	if (pRelationData != NULL)
	{
		return pRelationData->m_strNickname;
	}

	return XString();
}

XString CClientPlayer::GetGroupName( uint32 unGroupID )
{
	SGroupData* pGroupData = m_playerGroup.GetGroup(unGroupID);
	if (pGroupData != NULL)
	{
		return pGroupData->m_strGroupName;
	}

	return XString();
}

XString CClientPlayer::GetGroupMemberName( uint32 unGroupID, uint32 unGroupMemberID )
{
	SGroupData* pGroupData = m_playerGroup.GetGroup(unGroupID);
	if (pGroupData != NULL)
	{
		SGroupMemeber* pMember = pGroupData->GetMember(unGroupMemberID);
		if (pMember != NULL)
		{
			return pMember->m_strName;
		}
	}

	return XString();
}
