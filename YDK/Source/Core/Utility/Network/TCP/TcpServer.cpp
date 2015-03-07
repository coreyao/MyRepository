#include "Network/tcp/TcpServer.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CTcpServerRunThread::CTcpServerRunThread()
: m_server(NULL)
{
}

CTcpServerRunThread::~CTcpServerRunThread()
{
}

void CTcpServerRunThread::Run()
{
	if (m_server != NULL)
	{
		m_server->RunOneStep();
	}

	MSleep(1);
}


void CTcpServer::ListenSocket::OnError(Core::Utility::int32 iErrCode, const Core::Utility::XChar *strDescription/* = 0*/)
{
	if (m_pServer != NULL)
	{
		m_pServer->OnError(iErrCode, strDescription);
	}
}

CTcpServer::CTcpServer()
: m_uiMaxConnect(0)
, m_uiPort(0)
, m_uiSocketBuffSize(0)
, m_isRunning(false)
{
	m_listen.m_pServer = this;
	m_runThread.SetServer(this);
}

CTcpServer::~CTcpServer()
{
	if (m_isRunning)
	{
		m_isRunning = false;
		m_listen.Close();
		m_runThread.SetStop(true);
		ReleaseAllSession();
	}
}

void CTcpServer::Init( uint16 uPort, uint32 uMaxConnect, XString strBindLocalIP /*= TEXT("")*/, uint32 uiSocketBuff /*= 1*1024*1024*/ )
{
	if (!m_isRunning)
	{
		m_uiPort = uPort;
		m_uiMaxConnect = uMaxConnect;
		m_strLocalIP = strBindLocalIP;
		m_uiSocketBuffSize = uiSocketBuff;
	}
}

bool CTcpServer::ListenSocketOpt()
{
	bool bSucceed = true;
	bSucceed = m_listen.SetNonBlocking(true);
	bSucceed = m_listen.SetNoDelay(true);
	bSucceed = m_listen.SetLinger(false);
	bSucceed = m_listen.SetSendBuffSize(m_uiSocketBuffSize);
	bSucceed = m_listen.SetRecvBuffSize(m_uiSocketBuffSize);

	return bSucceed;
}

bool CTcpServer::Start( bool bNewThread /*= true*/, int32 nThreadPriority /*= THREAD_PRIORITY_HIGHEST*/ )
{
	if (!m_isRunning)
	{
		bool bSucceed = true;
		bSucceed = StartListen();
		if (bNewThread && bSucceed)
		{
			m_runThread.Start(true);
		}

		m_isRunning = true;

		if (!bSucceed)
		{
			//OnError();
			End();
		}

		return bSucceed;
	}

	//OnError();
	return true;
}

void CTcpServer::End()
{
	if (m_isRunning)
	{
		m_isRunning = false;
		StopListen();
		m_runThread.SetStop(true);
		ReleaseAllSession();
	}
}

bool CTcpServer::StartListen()
{
	bool bSucceed = true;
	bSucceed = m_listen.Create();
	bSucceed = m_listen.Bind(m_uiPort, m_strLocalIP);
	if (!bSucceed)
	{
		return false;
	}

	bSucceed = ListenSocketOpt();
	bSucceed = m_listen.Listen();

	return bSucceed;
}

void CTcpServer::StopListen()
{
	m_listen.Close();
}

void CTcpServer::Run()
{
	if (m_isRunning)
	{
		RunOneStep();
	}
}

CIOCPSession* CTcpServer::CreateIOCPSession()
{
	return new CIOCPSession;
}

void CTcpServer::ReleaseSession( CIOCPSession* pIOCPSession )
{
	delete pIOCPSession;
}

void CTcpServer::OnAcceptNewSession( CIOCPSession* pIOCPSession )
{

}

void CTcpServer::OnRunOneStep()
{

}

void CTcpServer::AcceptProcess()
{
	SOCKET sock = INVALID_SOCKET;
	while (m_sessionList.size() < m_uiMaxConnect && m_listen.Accept(sock))
	{
		CIOCPSession* pSession = CreateIOCPSession();
		pSession->Attach(sock, CIOCPSession::EState_Connected);

		if (m_IOWorkThread.GetInstance().GetIOCP().AssociateDevice(reinterpret_cast<HANDLE>(pSession->GetSocket()), reinterpret_cast<ULONG_PTR>(pSession)))
		{
			pSession->SetBindIOCP(&m_IOWorkThread.GetInstance().GetIOCP());
			pSession->EnableSendData(true);
			pSession->EnableRecvData(true);
		}
		else
		{
			//pSession->OnError()
			pSession->Close();
			ReleaseSession(pSession);
			return;
		}

		m_sessionList.insert(pSession);
		OnAcceptNewSession(pSession);
		sock = INVALID_SOCKET;
	}
}

void CTcpServer::CloseSessionProcess()
{
	for (std::set<CIOCPSession*>::iterator itr = m_sessionList.begin();
		itr != m_sessionList.end();
		++itr)
	{
		CIOCPSession* pSession = *itr;
		if (pSession != NULL && pSession->WillBeClose())
		{
			pSession->CloseSocket();
		}
	}
}

void CTcpServer::ReleaseSessionProcess()
{
	for (std::set<CIOCPSession*>::iterator itr = m_sessionList.begin();
		itr != m_sessionList.end();)
	{
		bool bReleaseItr = false;

		CIOCPSession* pSession = *itr;
		if (pSession == NULL)
		{
			bReleaseItr = true;
		}
		else
		{
			if (pSession->WillBeRelease())
			{
				m_willReleaseList.insert(pSession);
				bReleaseItr = true;
			}
		}

		if (bReleaseItr)
		{
			itr = m_sessionList.erase(itr);
		}
		else
		{
			++itr;
		}
	}

	for (std::set<CIOCPSession*>::iterator itr = m_willReleaseList.begin();
		itr != m_willReleaseList.end();)
	{
		bool bReleaseItr = false;

		CIOCPSession* pSession = *itr;
		if (pSession == NULL)
		{
			bReleaseItr = true;
		}
		else
		{
			if (pSession->CanBeRelease())
			{
				ReleaseSession(pSession);
				bReleaseItr = true;
			}
		}

		if (bReleaseItr)
		{
			itr = m_willReleaseList.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void CTcpServer::ReleaseAllSession()
{
	for (std::set<CIOCPSession*>::iterator itr = m_sessionList.begin();
		itr != m_sessionList.end();)
	{
		CIOCPSession* pSession = *itr;
		if (pSession == NULL)
		{
			itr = m_sessionList.erase(itr);
		}
		else
		{
			pSession->SetRelease();
		}

		++itr;
	}

	while (!m_sessionList.empty() || !m_willReleaseList.empty())
	{
		ReleaseSessionProcess();
		::Sleep(500);
	}
}

void CTcpServer::RunOneStep()
{
	AcceptProcess();
	CloseSessionProcess();
	ReleaseSessionProcess();
	OnRunOneStep();
}

void CTcpServer::OnError( int32 nError, const XChar* pErrDes /*= NULL*/ )
{
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
