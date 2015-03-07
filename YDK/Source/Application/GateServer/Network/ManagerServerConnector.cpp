#include <iostream>

#include "ManagerServerConnector.h"
#include "MsgDef.h"
#include "Singleton.h"
#include "GateServer.h"

using Core::Utility::TSingleton;

CManagerServerConnector::CManagerServerConnector()
{

}

CManagerServerConnector::~CManagerServerConnector()
{

}

void CManagerServerConnector::ProcessMsg()
{
	if (m_pSession == NULL || m_pSession->IsClosed())
	{
		CheckConnection();
		return;
	}

	if (m_pSession->IsConnected())
	{
		if (TSingleton<CGateServer>::GetInstance().GetGateID() == 0)
		{
			MsgReqGateLine msg;
			m_pSession->SendMsgPack(&msg);
		}
	}

	SMsg* pMsg = m_pSession->PopMsgPack();
	if (pMsg != NULL)
	{
		switch (pMsg->GetType())
		{
		case EWorldMsg_M2G_AckGateLine:
			{
				MsgAckGateLine* ackMsg = static_cast<MsgAckGateLine*>(pMsg);
				TSingleton<CGateServer>::GetInstance().SetGateID(ackMsg->m_unGateLine);
				std::cout << "Managerserver Connected" << std::endl;
			}
			break;
		case EWorldMsg_AckRegisterAccount:
			{
				MsgAckRegisterAccount* pAckMsg = static_cast<MsgAckRegisterAccount*>(pMsg);
				if (pAckMsg != NULL)
				{
					TSingleton<CGateServer>::GetInstance().OnAckRegisterAccount(pAckMsg);
				}
			}
			break;
		case EWorldMsg_S2C_AckLoginCheck:
			{
				MsgAckLoginCheck* ackMsg = static_cast<MsgAckLoginCheck*>(pMsg);
				if (ackMsg != NULL)
				{
					TSingleton<CGateServer>::GetInstance().OnAckLoginCheck(ackMsg);
				}
			}
			break;
		case EWorldMsg_M2C_AckEnterWorld:
			{
				MsgAckEnterWorld* pAckMsg = static_cast<MsgAckEnterWorld*>(pMsg);
				TSingleton<CGateServer>::GetInstance().OnAckPlayerEnterWorld(pAckMsg);
			}
			break;
		default:
			{
				TSingleton<CGateServer>::GetInstance().OnDispatchMsgToClient(pMsg);
			}
			break;
		}
	}
}