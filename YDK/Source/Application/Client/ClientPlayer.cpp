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