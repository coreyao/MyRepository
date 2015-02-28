#include <iostream>

#include "Lockers/Guard.h"
#include "Network/ManagerServerListener.h"
#include "MsgDef.h"
#include "Network/Session/ServerSession.h"
#include "Singleton.h"
#include "DBServer.h"
#include "CommonDataStruct/PlayerInfo_Struct.h"

using Core::Utility::uint8;
using Core::Utility::CServerSession;
using Core::Utility::TSingleton;
using Core::Utility::SMsgDynamic;
using Core::Utility::CGuard;

CManagerServerListener::CManagerServerListener()
{
}

CManagerServerListener::~CManagerServerListener()
{
}

void CManagerServerListener::ProcessMsg()
{
	CGuard guard(&m_lock);
	std::vector<CServerSession*>::iterator iter = m_managerSessions.begin();
	for (; iter != m_managerSessions.end();)
	{
		CServerSession* managerSession = *iter;
		if (managerSession == NULL || managerSession->IsClosed())
		{
			iter = m_managerSessions.erase(iter);
			managerSession->SetRelease();
			continue;
		}

		SMsg* pMsg = managerSession->PopMsgPack();
		if (pMsg != NULL)
		{
			switch (pMsg->GetType())
			{
			case EWorldMsg_ReqRegisterAccount:
				{
					MsgReqRegisterAccount* pReqLoginCheck = static_cast<MsgReqRegisterAccount*>(pMsg);
					if (pReqLoginCheck != NULL)
					{
						MsgAckRegisterAccount ackMsg;
						ackMsg.m_challengeID = pReqLoginCheck->m_challengeID;
						ackMsg.m_gateID = pReqLoginCheck->m_gateID;
						bool bResult = TSingleton<CDBServer>::GetInstance().OnRegisterAccount(pReqLoginCheck->m_account,
							pReqLoginCheck->m_password, pReqLoginCheck->m_nickname);
						if (bResult)
						{
							ackMsg.m_result = MsgAckRegisterAccount::EResult_Success;
						}
						else
						{
							ackMsg.m_result = MsgAckRegisterAccount::EResult_Failed;
						}

						managerSession->SendMsgPack(&ackMsg);
					}
				}
				break;
			case EWorldMsg_C2S_ReqLoginCheck:
				{
					MsgReqLoginCheck* pReqLoginCheck = static_cast<MsgReqLoginCheck*>(pMsg);
					if (pReqLoginCheck != NULL)
					{
						MsgAckLoginCheck ackMsg;
						ackMsg.accountID = TSingleton<CDBServer>::GetInstance()
							.OnPlayerLoginCheck(pReqLoginCheck->account, pReqLoginCheck->password);
						if (ackMsg.accountID != INVALID_ACCOUNT_ID)
						{
							ackMsg.result = MsgAckLoginCheck::EResult_Success;
							memcpy(ackMsg.account, pReqLoginCheck->account, MAX_ACCOUNT_LEN);
						}
						else
						{
							ackMsg.result = MsgAckLoginCheck::EResult_IncorrectAccOrPwd;
						}

						ackMsg.challengeID = pReqLoginCheck->m_challengeID;
						ackMsg.gateID = pReqLoginCheck->m_gateID;
						managerSession->SendMsgPack(&ackMsg);
					}
				}
				break;
			case EWorldMsg_C2S_ReqEnterWorld:
				{
					MsgReqEnterWorld* pReqEnterWorld = static_cast<MsgReqEnterWorld*>(pMsg);

					SPlayerBaseInfo baseInfo;
					uint32 friendsInfoLen = 0;
					char friendsInfo[MAX_RELATIONDATA_LEN] = {0};
					uint32 groupsInfoLen = 0;
					char groupsInfo[MAX_GROUPDATA_LEN] = {0};

					DBMsgAckEnterWorld ackMsg;
					ackMsg.SetPlayerID(pReqEnterWorld->GetPlayerID());
					if (TSingleton<CDBServer>::GetInstance().OnPlayerEnterWorld(pReqEnterWorld->GetPlayerID(),
						baseInfo, friendsInfoLen, friendsInfo, groupsInfoLen, groupsInfo))
					{
						ackMsg.m_result = DBMsgAckEnterWorld::EResult_Success;
						memcpy(ackMsg.m_baseInfo.m_nickName, baseInfo.m_nickName, MAX_NICKNAME_LEN);
						ackMsg.m_friendsInfoLen = friendsInfoLen;
						memcpy(ackMsg.m_friendsInfo, friendsInfo, friendsInfoLen);
						ackMsg.m_groupsInfoLen = groupsInfoLen;
						memcpy(ackMsg.m_groupsInfo, groupsInfo, groupsInfoLen);
					}
					else
					{
						ackMsg.m_result = DBMsgAckEnterWorld::EResult_PlayerNotFound;
					}

					managerSession->SendMsgPack(&ackMsg);
				}
				break;
			case EPlayerMsg_M2D_SaveRelationData:
				{
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					TSingleton<CDBServer>::GetInstance().OnSavePlayerRelation(pMsg->GetPlayerID(), cPacket.GetDataSize(), cPacket.GetData());
				}
				break;
			case EGroupMsg_M2D_ReqLoadGroup:
				{
					std::vector<SGroupData> vecGroups;
					TSingleton<CDBServer>::GetInstance().OnLoadGroupsDataInfo(vecGroups);

					CDynamicPacket cPacket(EGroupMsg_D2M_AckLoadGroup);
					cPacket << vecGroups;
					managerSession->SendMsgPack(cPacket.GetMsg());
				}
				break;
			case EGroupMsg_C2S_ReqCreate:
				{
					SGroupData groupData;
					MsgReqCreateGroup* reqMsg = static_cast<MsgReqCreateGroup*>(pMsg);
					if (TSingleton<CDBServer>::GetInstance().OnReqCreateGroup(reqMsg->m_groupName, groupData))
					{
						CDynamicPacket cPacket(EGroupMsg_D2M_AckCreate);
						cPacket.GetMsg()->SetPlayerID(reqMsg->GetPlayerID());
						cPacket << groupData;
						managerSession->SendMsgPack(cPacket.GetMsg());
					}
				}
				break;
			case EPlayerMsg_M2D_SaveGroupData:
				{
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					TSingleton<CDBServer>::GetInstance().OnSavePlayerGroups(pMsg->GetPlayerID(), cPacket.GetDataSize(), cPacket.GetData());
				}
				break;
			case EGroupMsg_M2D_SaveGroupsInfo:
				{
					uint32 groupID = 0;
					CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
					cPacket << groupID;
					cPacket.SetCurrentPosition(0);
					TSingleton<CDBServer>::GetInstance().OnSaveGroupsInfo(groupID, cPacket.GetDataSize(), cPacket.GetData());
				}
				break;
			default:
				break;
			}
		}

		++iter;
	}
}

void CManagerServerListener::OnAcceptNewSession( CIOCPSession* pIOCPSession )
{
	std::cout << "Manager Server Connected" << std::endl;
	CGuard guard(&m_lock);
	m_managerSessions.push_back(static_cast<CServerSession*>(pIOCPSession));
}

CIOCPSession* CManagerServerListener::CreateIOCPSession()
{
	return new CServerSession;
}
