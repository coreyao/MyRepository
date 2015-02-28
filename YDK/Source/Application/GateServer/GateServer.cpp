#include <iostream>
#include <iterator>
#include "GateServer.h"
#include "Network/ClientListener.h"
#include "NetWork/ManagerServerConnector.h"
#include "Lockers/Guard.h"
#include "TimeInfo/TimeInfo.h"
#include "msini/CMSINIReader.h"

#define MAX_RECV_FLAG 2000

using Core::Utility::CGuard;
using Core::Utility::CTimeInfo;
using Core::Utility::CMSINIReader;

CGateServer::CGateServer()
: m_unLastChallengeID(1)
, m_unLastSyncTime(0)
, m_unGateID(0)
{
}

CGateServer::~CGateServer()
{
}

void CGateServer::Init()
{
	//Load Configuration File
	TSingleton<CMSINIReader>::GetInstance().SetFileName(TEXT(".\\ServerConfig.ini"));

	XString gateServerIP = TSingleton<CMSINIReader>::GetInstance().GetFieldStringValue(TEXT("GateServer"), TEXT("GateServerIP"));
	int32 gateServerPort = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("GateServer"), TEXT("GateServerPort"));
	uint32 gateServerMaxConnections = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("GateServer"), TEXT("GateServerMaxConnections"));

	XString managerServerIP = TSingleton<CMSINIReader>::GetInstance().GetFieldStringValue(TEXT("ManagerServer"), TEXT("ManagerServerIP"));
	int32 managerServerPort = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("ManagerServer"), TEXT("ManagerServerPort"));

	TSingleton<CClientListener>::GetInstance().Init(static_cast<uint16>(gateServerPort), gateServerMaxConnections, gateServerIP);
	TSingleton<CClientListener>::GetInstance().Start();

	TSingleton<CManagerServerConnector>::GetInstance().Init(managerServerIP, managerServerPort);
}

void CGateServer::Process()
{
	SyncSessions();
	
	TSingleton<CClientListener>::GetInstance().ProcessMsg();
	TSingleton<CManagerServerConnector>::GetInstance().ProcessMsg();

	ProcessLoginClientMsg();
	ProcessClientMsg();
}

void CGateServer::OnAcceptNewSession(CClientSessionExt* newSession)
{
	if (newSession != NULL)
	{
		CGuard guard(&m_lock);
		while (m_tempSessions.find(m_unLastChallengeID) != m_tempSessions.end())
		{
			++m_unLastChallengeID;
		}

		m_tempSessions[m_unLastChallengeID] = newSession;
		newSession->SetChallengeID((m_unLastChallengeID));
		++m_unLastChallengeID;


		if (m_unLastChallengeID == 0xFFFFFFFF)
		{
			m_unLastChallengeID = 0;
		}
	}
}

void CGateServer::SyncSessions()
{
	CTimeInfo curTime;
	if (curTime.GetNowTime() - m_unLastSyncTime > 1)
	{
		{
			CGuard guard(&m_lock);
			MAPCLIENTSEXT::iterator iterTemp = m_tempSessions.begin();
			for (; iterTemp != m_tempSessions.end();)
			{
				if (iterTemp->second == NULL)
				{
					iterTemp = m_tempSessions.erase(iterTemp);
					continue;
				}

				if (iterTemp->second->IsClosed())
				{
					iterTemp->second->SetRelease();
					iterTemp = m_tempSessions.erase(iterTemp);
					continue;
				}

				if (iterTemp->second->GetRecvFlag() > MAX_RECV_FLAG)
				{
					iterTemp->second->Close();
					++iterTemp;
					continue;
				}

				iterTemp->second->AddRecvFlag();
				++iterTemp;
			}
			m_loginSessions = m_tempSessions;
		}
		
		MAPCLIENTS::iterator iterChar = m_charactors.begin();
		for (; iterChar != m_charactors.end();)
		{
			if (iterChar->second == NULL || iterChar->second->GetClientSession() == NULL )
			{
				iterChar = m_charactors.erase(iterChar);
				continue;
			}

			if (iterChar->second->GetClientSession()->IsClosed())
			{
				//if the charactor is in world
				if (iterChar->second->IsInWorld())
				{
					MsgReqExitWorld reqMsg;
					reqMsg.SetPlayerID(iterChar->first);
					TSingleton<CManagerServerConnector>::GetInstance().SendMsg(&reqMsg);
					iterChar->second->SetInWorld(false);
				}

				iterChar->second->GetClientSession()->SetRelease();
				SAFE_DELETE(iterChar->second);
				iterChar = m_charactors.erase(iterChar);
				continue;
			}

			if (iterChar->second->GetClientSession()->GetRecvFlag() > MAX_RECV_FLAG)
			{
				iterChar->second->GetClientSession()->Close();
				++iterChar;
				continue;
			}
			
			iterChar->second->GetClientSession()->AddRecvFlag();
			++iterChar;
		}

		m_unLastSyncTime = curTime.GetNowTime();
	}
}

void CGateServer::ProcessLoginClientMsg()
{
	MAPCLIENTSEXT::iterator iter = m_loginSessions.begin();
	for (; iter != m_loginSessions.end();)
	{
		CClientSessionExt* pClientSessionExt = iter->second;
		if (pClientSessionExt == NULL 
			|| pClientSessionExt->IsClosed()
			|| pClientSessionExt->GetRecvFlag() > MAX_RECV_FLAG)
		{
			iter = m_loginSessions.erase(iter);
			continue;
		}

		for (int i = 0; i < 3; ++i)
		{
			SMsg* pMsg = pClientSessionExt->PopMsgPack();
			if (pMsg == NULL)
			{
				break;
			}
			else
			{
				switch (pMsg->GetType())
				{
				case ETestMSg_TestBigPacket:
					{
						MsgTestBigPacket* pTestBigMsg = static_cast<MsgTestBigPacket*>(pMsg);
					}
					break;
				case EWorldMsg_ReqRegisterAccount:
					{
						MsgReqRegisterAccount* pRegisterMsg = static_cast<MsgReqRegisterAccount*>(pMsg);
						pRegisterMsg->m_challengeID = pClientSessionExt->GetChallengeID();
						pRegisterMsg->m_gateID = GetGateID();
						TSingleton<CManagerServerConnector>::GetInstance().SendMsg(pRegisterMsg);
					}
					break;
				case EWorldMsg_C2S_ReqLoginCheck:
					{
						MsgReqLoginCheck* pReqLoginCheck = static_cast<MsgReqLoginCheck*>(pMsg);
						pReqLoginCheck->m_challengeID = pClientSessionExt->GetChallengeID();
						pReqLoginCheck->m_gateID = GetGateID();
						TSingleton<CManagerServerConnector>::GetInstance().SendMsg(pReqLoginCheck);
					}
					break;
				case EWorldMsg_C2S_ReqEnterWorld:
					{
						MAPCLIENTS::iterator iter = m_charactors.find(pMsg->GetPlayerID());
						if (iter != m_charactors.end())
						{
							//close the session
							if (iter->second == NULL || iter->second->GetClientSession() == NULL)
							{
								return;
							}
							iter->second->GetClientSession()->Close();

							//send exit world message
							if (iter->second->IsInWorld())
							{
								MsgReqExitWorld reqMsg;
								reqMsg.SetPlayerID(iter->first);
								TSingleton<CManagerServerConnector>::GetInstance().SendMsg(&reqMsg);
								iter->second->SetInWorld(false);
							}

			/*				MsgAckEnterWorld ackMsg;
							ackMsg.SetPlayerID(pMsg->GetPlayerID());
							ackMsg.m_result = static_cast<uint8>(MsgAckEnterWorld::EResult_PlayerAlreadyInWorld);
							pClientSessionExt->SendMsgPack(&ackMsg);*/
						}
						
						TSingleton<CManagerServerConnector>::GetInstance().SendMsg(pMsg);
					}
					break;
				default:
					{
						TSingleton<CManagerServerConnector>::GetInstance().SendMsg(pMsg);
					}
					break;
				}
			}
		}

		++iter;
	}
}

void CGateServer::ProcessClientMsg()
{
	MAPCLIENTS::iterator iter = m_charactors.begin();
	for (; iter != m_charactors.end();)
	{
		if (iter->second == NULL 
			|| iter->second->GetClientSession() == NULL
			|| iter->second->GetClientSession()->IsClosed()
			|| iter->second->GetClientSession()->GetRecvFlag() > MAX_RECV_FLAG)
		{
			++iter;
			continue;
		}

		for (int i = 0; i < 5; ++i)
		{
			SMsg* pMsg = iter->second->PopMsgPack();
			if (pMsg == NULL)
			{
				break;
			}
			else
			{
				switch (pMsg->GetType())
				{
				case EWorldMsg_ReqExitWorld:
					{
						iter->second->GetClientSession()->Close();
						TSingleton<CManagerServerConnector>::GetInstance().SendMsg(pMsg);
						iter->second->SetInWorld(false);
					}
					break;
				default:
					{
						TSingleton<CManagerServerConnector>::GetInstance().SendMsg(pMsg);
					}
					break;
				}
			}
		}

		++iter;
	}
}

void CGateServer::OnAckLoginCheck( MsgAckLoginCheck* ackMsg )
{
	if (ackMsg != NULL)
	{
		SendMsgToClientByChallengeID(ackMsg, ackMsg->challengeID);
	}
}

void CGateServer::OnAckPlayerEnterWorld(MsgAckEnterWorld* pAckMsg)
{
	if (pAckMsg != NULL)
	{
		uint8 result = pAckMsg->m_result;
		uint32 challengeID = pAckMsg->m_unChallengeID;
		MAPCLIENTSEXT::iterator iter = m_loginSessions.find(challengeID);
		if (iter != m_loginSessions.end())
		{
			if (result == DBMsgAckEnterWorld::EResult_Success)
			{
				CGateClient* gateClient = new CGateClient;
				gateClient->SetAccountID(pAckMsg->GetPlayerID());
				gateClient->SetClientSession(iter->second);
				gateClient->SetInWorld(true);

				m_charactors.insert(std::make_pair(gateClient->GetAccountID(), gateClient));
				iter->second->SendMsgPack(pAckMsg);

				{
					CGuard guard(&m_lock);
					m_tempSessions.erase(iter->first);
				}
				m_loginSessions.erase(iter);
			}
			else
			{
				iter->second->SendMsgPack(pAckMsg);
			}
		}
	}
}

void CGateServer::SendMsgToClientByChallengeID(SMsg* msg, uint32 challengeID )
{
	MAPCLIENTSEXT::iterator iter = m_loginSessions.find(challengeID);
	if (iter != m_loginSessions.end())
	{
		iter->second->SendMsgPack(msg);
	}
}

void CGateServer::ProcessCommand()
{
	XString lineStr;
	while (getline(std::cin, lineStr))
	{
		std::vector<XString> strs = lineStr.SplitString();
		if (strs.size() == 1)
		{
			OnCommand(strs[0], std::vector<XString>());
		}
		else
		{
			std::vector<XString> params;
			copy(strs.begin() + 1, strs.end(), back_inserter(params));
			OnCommand(strs[0], params);
		}
	}
}

void CGateServer::OnCommand( const XString& strCmd, const std::vector<XString>& params )
{
	if (strCmd == "test")
	{
		std::cout << "test succeeded" << std::endl;
	}
}

void CGateServer::OnDispatchMsgToClient( SMsg* pMsg )
{
	MAPCLIENTS::iterator iter = m_charactors.find(pMsg->GetPlayerID());
	if (iter != m_charactors.end())
	{
		iter->second->GetClientSession()->SendMsgPack(pMsg);
	}
}

void CGateServer::OnAckRegisterAccount( MsgAckRegisterAccount* pAckMsg )
{
	if (pAckMsg != NULL)
	{
		SendMsgToClientByChallengeID(pAckMsg, pAckMsg->m_challengeID);
	}
}