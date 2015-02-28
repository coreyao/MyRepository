#include "Network/ServerConnector.h"
#include "MsgDef.h"
#include "Singleton.h"
#include "ClientGUI/ClientGUI.h"
#include "ClientGUI/ClientPlayer.h"
#include "ClientGUI/ChatHistoryManager.h"
#include "LogSystem.h"
#include "Network/Packet/DynamicPacket.h"
#include "CommonDataStruct/ChatData_Struct.h"
#include "ClientGUI/FileTransManager.h"

using Core::Utility::TSingleton;
using Core::Utility::SMsgDynamic;
using Core::Utility::CDynamicPacket;

CServerConnector::CServerConnector()
{
}

CServerConnector::~CServerConnector()
{
}

void CServerConnector::ProcessMsg()
{
	if (m_pSession == NULL || m_pSession->IsClosed())
	{
		CheckConnection();
		return;
	}

	TSingleton<CFileTransManager>::GetInstance().CheckFiles();

	SMsg* pMsg = m_pSession->PopMsgPack();
	if (pMsg != NULL)
	{
		if (pMsg->GetType() > EProtocolRange_FileMsg_Begin && pMsg->GetType() < EProtocolRange_FileMsg_End)
		{
			TSingleton<CFileTransManager>::GetInstance().ProcessMsg(pMsg);
		}
		else
		{
			switch (pMsg->GetType())
			{
			case EWorldMsg_AckRegisterAccount:
				{
					TSingleton<CClientGUI>::GetInstance().OnAckRegister(static_cast<MsgAckRegisterAccount*>(pMsg));
				}
				break;
			case EWorldMsg_S2C_AckLoginCheck:
				{
					bool bRet = TSingleton<CClientGUI>::GetInstance().OnAckLogin(static_cast<MsgAckLoginCheck*>(pMsg));
					if (bRet)
					{
						TSingleton<CClientGUI>::GetInstance().OnReqEnterWorld();
					}
				}
				break;
			case EWorldMsg_M2C_AckEnterWorld:
				{
					TSingleton<CClientGUI>::GetInstance().OnAckEnterWorld(static_cast<MsgAckEnterWorld*>(pMsg));
				}
				break;
			case EPlayerMsg_M2C_UpdateAllRelationData:
				{
					TSingleton<CClientGUI>::GetInstance().OnAckInitRelation(static_cast<SMsgDynamic*>(pMsg));
				}
				break;
			case EPlayerMsg_M2C_UpdateRelationData:
				{
					TSingleton<CClientGUI>::GetInstance().OnAckUpdateRelation(static_cast<SMsgDynamic*>(pMsg));
				}
				break;
			case EPlayerMsg_M2C_UpdateFriendGroupData:
				{
					TSingleton<CClientGUI>::GetInstance().OnAckUpdateFriendGroup(static_cast<SMsgDynamic*>(pMsg));
				}
				break;
			case EPlayerMsg_S2C_UpdateGroupMember:
				{
					TSingleton<CClientGUI>::GetInstance().OnAckUpdateGroupMember(static_cast<SMsgDynamic*>(pMsg));
				}
				break;
			case EPlayerMsg_S2C_UpdateGroup:
				{
					TSingleton<CClientGUI>::GetInstance().OnAckUpdateGroup(static_cast<SMsgDynamic*>(pMsg));
				}
				break;
			case EPlayerMsg_S2C_UpdateAllGroup:
				{
					TSingleton<CClientGUI>::GetInstance().OnAckInitGroups(static_cast<SMsgDynamic*>(pMsg));
				}
				break;
			case EWorldMsg_Chat:
				{
					SChatData chatData;
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					cPacket << chatData;

					if (chatData.m_unChannel == EChannel_Single)
					{
						TSingleton<CClientGUI>::GetInstance().OnRecvSingleChat(&chatData);
					}
					else
					{
						TSingleton<CClientGUI>::GetInstance().OnRecvGroupChat(&chatData);
					}
				}
				break;
			default:
				break;
			}
		}
	}
}
