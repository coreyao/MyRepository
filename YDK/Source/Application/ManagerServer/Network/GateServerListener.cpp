#include <iostream>

#include "GateServerListener.h"
#include "MsgDef.h"
#include "Lockers/Guard.h"
#include "Network/DBServerConnector.h"
#include "Singleton.h"
#include "CharactorManager/CharactorManager.h"
#include "ChatManager/ChatManager.h"
#include "GroupManager/GroupManager.h"
#include "RelationManager/RelationManager.h"
#include "FileTransManager/FileTransManager.h"

using Core::Utility::uint8;
using Core::Utility::CGuard;
using Core::Utility::XString;
using Core::Utility::TSingleton;

CGateServerListener::CGateServerListener()
{
}

CGateServerListener::~CGateServerListener()
{

}

void CGateServerListener::ProcessMsg()
{
	CGuard guard(&m_lock);
	std::vector<CGateSession*>::iterator iter = m_gateSessions.begin();
	for (; iter != m_gateSessions.end(); )
	{
		CGateSession* gateSession = *iter;;
		if (gateSession == NULL || gateSession->IsClosed())
		{
			iter = m_gateSessions.erase(iter);
			gateSession->SetRelease();
			continue;
		}

		SMsg* pMsg = gateSession->PopMsgPack();
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
			else if (pMsg->GetType() > EProtocolRange_FileMsg_Begin && pMsg->GetType() < EProtocolRange_FileMsg_End)
			{
				TSingleton<CFileTransManager>::GetInstance().ProcessMsg(pMsg);
			}
			else
			{
				switch (pMsg->GetType())
				{
				case EWorldMsg_G2M_ReqGateLine:
					{
						if (gateSession->GetGateID() != 0)
						{
							return;
						}

						for (uint8 i = 1; i < 255; ++i)
						{
							bool isUsed = false;
							std::vector<CGateSession*>::iterator it = m_gateSessions.begin();
							for (; it != m_gateSessions.end(); ++it)
							{
								CGateSession* pSession = *it;
								if (pSession != NULL && pSession->GetGateID() == i)
								{
									isUsed = true;
									break;
								}
							}

							if (!isUsed)
							{
								gateSession->SetGateID(i);
								MsgAckGateLine ackMsg;
								ackMsg.m_unGateLine = gateSession->GetGateID();
								gateSession->SendMsgPack(&ackMsg);	
								std::cout << "New Gate Allocated: " << gateSession->GetGateID() << std::endl;
								break;
							}
						}
					}
					break;
				case EWorldMsg_C2S_ReqLoginCheck:
				case EWorldMsg_C2S_ReqEnterWorld:
				case EWorldMsg_ReqRegisterAccount:
					{
						TSingleton<CDBServerConnector>::GetInstance().SendMsg(pMsg);
					}
					break;
				case EWorldMsg_Chat:
					{
						TSingleton<CChatManager>::GetInstance().ProcessMsg(pMsg);
					}
					break;
				case EWorldMsg_ReqExitWorld:
					{						
						TSingleton<CCharactorManager>::GetInstance().OnPlayerLeaveWorld(pMsg->GetPlayerID());
					}
					break;
				default:
					break;
				}
			}
		}

		++iter;
	}
}

void CGateServerListener::OnAcceptNewSession( CIOCPSession* pIOCPSession )
{
	CGuard guard(&m_lock);
	m_gateSessions.push_back(static_cast<CGateSession*>(pIOCPSession));
}

CIOCPSession* CGateServerListener::CreateIOCPSession()
{
	return new CGateSession;
}

void CGateServerListener::SendMsg( SMsg* msg, uint32 gateID /*= 0*/ )
{
	if (gateID == 0)
	{
		CGuard guard(&m_lock);
		for (uint32 i = 0; i < m_gateSessions.size(); ++i)
		{
			if (m_gateSessions[i] != NULL && !m_gateSessions[i]->IsClosed())
			{
				m_gateSessions[i]->SendMsgPack(msg);
			}
		}
	}
	else
	{
		CGuard guard(&m_lock);
		for (uint32 i = 0; i < m_gateSessions.size(); ++i)
		{
			if (m_gateSessions[i] != NULL && !m_gateSessions[i]->IsClosed()
				&& m_gateSessions[i]->GetGateID() == gateID)
			{
				m_gateSessions[i]->SendMsgPack(msg);
			}
		}
	}
}