#include <iostream>

#include "Singleton.h"
#include "Network/ClientListener.h"
#include "GateServer.h"

using Core::Utility::TSingleton;

CClientListener::CClientListener()
{
}

CClientListener::~CClientListener()
{
}

void CClientListener::OnAcceptNewSession( CIOCPSession* pIOCPSession )
{
	if (pIOCPSession != NULL)
	{
		TSingleton<CGateServer>::GetInstance().OnAcceptNewSession(static_cast<CClientSessionExt*>(pIOCPSession));
	}
}

Core::Utility::uint32 CClientListener::GetOnlineClientsCount()
{
	return m_sessionList.size();
}

void CClientListener::ProcessMsg()
{
	//std::cout << GetOnlineClientsCount() << std::endl;
}

CIOCPSession* CClientListener::CreateIOCPSession()
{
	return new CClientSessionExt;
}
