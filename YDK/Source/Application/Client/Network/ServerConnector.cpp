#include <iostream>

#include "Network/ServerConnector.h"
#include "MsgDef.h"
#include "Singleton.h"
#include "ClientPlayer.h"
#include "LogSystem.h"
#include "Network/Packet/DynamicPacket.h"
#include "CommonDataStruct/ChatData_Struct.h"
#include "ChatHistoryManager.h"
#include "FileTransManager.h"

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
					MsgAckRegisterAccount* ackMsg = static_cast<MsgAckRegisterAccount*>(pMsg);
					if (ackMsg != NULL && ackMsg->m_result == MsgAckRegisterAccount::EResult_Success)
					{
						Core::LOG("Register Success");
					}
					else
					{
						Core::LOG("Register Failed");
					}
				}
				break;
			case EWorldMsg_S2C_AckLoginCheck:
				{
					MsgAckLoginCheck* ackMsg = static_cast<MsgAckLoginCheck*>(pMsg);
					if (ackMsg != NULL && ackMsg->result == MsgAckLoginCheck::EResult_Success)
					{
						TSingleton<CClientPlayer>::GetInstance().SetAccountID(ackMsg->accountID);
						Core::LOG("Login Success");
					}
				}
				break;
			case EWorldMsg_M2C_AckEnterWorld:
				{

					MsgAckEnterWorld* pAckMsg = static_cast<MsgAckEnterWorld*>(pMsg);
					uint8 result = pAckMsg->m_result;
					uint32 challengeID = pAckMsg->m_unChallengeID;
					if (result == MsgAckEnterWorld::EResult_PlayerAlreadyInWorld)
					{
						Core::LOG("Player Already In World");
					}
					else if (result == MsgAckEnterWorld::EResult_PlayerNotFound)
					{
						Core::LOG("Player Not Found");
					}
					else if (result == MsgAckEnterWorld::EResult_Success)
					{
						TSingleton<CClientPlayer>::GetInstance().SetNickName(pAckMsg->m_playerBaseInfo.m_nickName);
						Core::LOG(XString().Format("Player %s Enter World", TSingleton<CClientPlayer>::GetInstance().GetNickName().c_str()).c_str());
					}
				}
				break;
			case EPlayerMsg_M2C_UpdateAllRelationData:
				{
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
					{
						SRelation relation;
						relation.SerializeClient(cPacket);
						TSingleton<CClientPlayer>::GetInstance().SetRelation(relation);
						Core::LOG("Player Relation Inited");
					}
				}
				break;
			case EPlayerMsg_M2C_UpdateRelationData:
				{
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
					{
						uint8 oper = 0;
						SRelationData relationData;
						cPacket << oper;
						relationData.SerializeClient(cPacket);

						SRelation& playerRealtion = TSingleton<CClientPlayer>::GetInstance().GetRelation();
						if (oper == SRelation::EOPER_AddRelation)
						{
							playerRealtion.AddRelation(relationData);
						}
						else if (oper == SRelation::EOPER_DelRelation)
						{
							playerRealtion.DelRelation(relationData);
						}
					}
				}
				break;
			case EPlayerMsg_S2C_UpdateGroupMember:
				{
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
					{
						uint8 oper = 0;
						uint32 groupID = 0;
						SGroupMemeber groupMemeber;
						cPacket << oper << groupID;
						groupMemeber.SerializeClient(cPacket);

						SGroups& groups = TSingleton<CClientPlayer>::GetInstance().GetGroups();
						SGroupData* pGroup = groups.GetGroup(groupID);
						if (pGroup != NULL)
						{
							if (oper == SGroupData::EOPER_JoinGroup)
							{
								pGroup->JoinGroup(groupMemeber);
							}
							else
							{
								pGroup->LeaveGroup(groupMemeber);
							}
						}
					}
				}
				break;
			case EPlayerMsg_S2C_UpdateGroup:
				{
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
					{
						uint8 oper = 0;
						SGroupData groupData;
						cPacket << oper;
						groupData.SerializeClient(cPacket);

						SGroups& groups = TSingleton<CClientPlayer>::GetInstance().GetGroups();
						if (oper == SGroups::EOPER_AddGroup)
						{
							groups.AddGroup(groupData);
						}
						else
						{
							groups.DelGroup(groupData);
						}
					}
				}
				break;
			case EPlayerMsg_S2C_UpdateAllGroup:
				{
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
					{
						SGroups groups;
						groups.SerializeClient(cPacket);
						TSingleton<CClientPlayer>::GetInstance().SetGroups(groups);
						Core::LOG("Player Groups Inited");
					}
				}
				break;
			case EWorldMsg_Chat:
				{
					SChatData chatData;
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					cPacket << chatData;

					Core::LOG(XString().Format("Player %s:%s", chatData.m_strSenderName.c_str(), chatData.m_strContent.c_str()).c_str());
					TSingleton<CChatHistoryManager>::GetInstance().Save(chatData, false);
				}
				break;
			default:
				break;
			}
		}
	}
}
