#include "Network/Worker/NetworkConnector.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CNetworkConnector::CNetworkConnector()
: m_uPort(0)
, m_pSession(NULL)
{
}

CNetworkConnector::~CNetworkConnector()
{
	if (m_pSession != NULL)
	{
		m_pSession->Close();
		m_pSession->SetRelease();
	}
}

void CNetworkConnector::Init( const XString& strIP, uint16 uPort )
{
	m_strIP = strIP;
	m_uPort = uPort;
}

void CNetworkConnector::CheckConnection()
{
	if (m_pSession == NULL)
	{
		m_pSession = new CClientSession;	
	}

	m_pSession->Connect(m_strIP, m_uPort);
}

bool CNetworkConnector::SendMsg( SMsg* msg )
{
	if (m_pSession != NULL && m_pSession->IsConnected())
	{
		return	m_pSession->SendMsgPack(msg);
	}

	return false;
}

bool CNetworkConnector::IsConnected()
{
	if (m_pSession != NULL && m_pSession->IsConnected())
	{
		return true;
	}

	return false;
}

bool CNetworkConnector::IsConnecting()
{
	if (m_pSession != NULL && m_pSession->IsConnecting())
	{
		return true;
	}

	return false;
}

void CNetworkConnector::Close()
{
	if (m_pSession != NULL)
	{
		m_pSession->Close();
	}
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)