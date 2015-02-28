#include "CharactorManager/GamePlayer.h"
#include "MsgDef.h"
#include "Singleton.h"
#include "Network/GateServerListener.h"
#include "AccountManager/AccountManager.h"
#include "CharactorManager/CharactorManager.h"
#include "Network/DBServerConnector.h"
#include "GroupManager/GroupManager.h"

using Core::Utility::TSingleton;

CGamePlayer::CGamePlayer()
: m_unPlayerID(0)
, m_unGateID(0)
{
}

CGamePlayer::~CGamePlayer()
{
}

XString CGamePlayer::GetNickName()
{
	return m_baseInfo.m_nickName;
}

void CGamePlayer::SetNickName( const XString& nickName )
{
	memcpy(m_baseInfo.m_nickName, nickName.c_str(), nickName.length());
}
