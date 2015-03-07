#include <iostream>

#include "DBServerConnector.h"
#include "MsgDef.h"
#include "Singleton.h"
#include "ManagerServer.h"
#include "CharactorManager/CharactorManager.h"
#include "GroupManager/GroupManager.h"
#include "RelationManager/RelationManager.h"
#include "Network/GateServerListener.h"

using Core::Utility::TSingleton;

CDBServerConnector::CDBServerConnector()
{

}

CDBServerConnector::~CDBServerConnector()
{

}

void CDBServerConnector::ProcessMsg()
{
	if (m_pSession == NULL || m_pSession->IsClosed())
	{
		CheckConnection();
		return;
	}

	if (m_pSession->IsConnected())
	{
	}

	SMsg* pMsg = m_pSession->PopMsgPack();
	if (pMsg != NULL)
	{
		/*if (pMsg->GetType() > EProtocolRange_PlayerMsg_Begin && pMsg->GetType() < EProtocolRange_PlayerMsg_End)
		{
		TSingleton<CCharactorManager>::GetInstance().ProcessMsg(pMsg);
		}
		else */if (pMsg->GetType() > EProtocolRange_GroupMsg_Begin && pMsg->GetType() < EProtocolRange_GroupMsg_End)
		{
			TSingleton<CGroupManager>::GetInstance().ProcessMsg(pMsg);
		}
		else if (pMsg->GetType() > EProtocolRange_RelationMsg_Begin && pMsg->GetType() < EProtocolRange_RelationMsg_End)
		{
			TSingleton<CRelationManager>::GetInstance().ProcessMsg(pMsg);
		}
		else
		{
			switch (pMsg->GetType())
			{
			case EWorldMsg_AckRegisterAccount:
				{
					MsgAckRegisterAccount* ackMsg = static_cast<MsgAckRegisterAccount*>(pMsg);
					if (ackMsg != NULL)
					{
						TSingleton<CGateServerListener>::GetInstance().SendMsg(ackMsg, ackMsg->m_gateID);
					}
				}
				break;
			case EWorldMsg_S2C_AckLoginCheck:
				{
					MsgAckLoginCheck* ackMsg = static_cast<MsgAckLoginCheck*>(pMsg);
					if (ackMsg != NULL)
					{
						TSingleton<CManagerServer>::GetInstance().OnAckLoginCheck(ackMsg);
					}
				}
				break;
			case EWorldMsg_D2M_AckEnterWorld:
				{
					DBMsgAckEnterWorld* ackMsg = static_cast<DBMsgAckEnterWorld*>(pMsg);
					if (ackMsg != NULL)
					{
						TSingleton<CManagerServer>::GetInstance().OnAckPlayerEnterWorld(ackMsg);
					}
				}
				break;
			default:
				break;
			}
		}
	}
}