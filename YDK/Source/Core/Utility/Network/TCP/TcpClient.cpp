#include "Network/tcp/TcpClient.h"
#include "LogSystem.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CTcpClientConnectionThread::CTcpClientConnectionThread()
: m_tcpClient(NULL)
{
}

CTcpClientConnectionThread::~CTcpClientConnectionThread()
{
}

void CTcpClientConnectionThread::Run()
{
	if (m_tcpClient != NULL && !m_tcpClient->IsConnected())
	{
		m_tcpClient->Connecting();
	}

	MSleep(1);
}

CTcpClient::CTcpClient()
: m_runIOBuff(1)
, m_bEnableRun(false)
, m_uiSendBuffSize(SO_MAX_MSG_SIZE)
, m_uiRecvBuffSize(SO_MAX_MSG_SIZE)
, m_uRemotePort(0)
{
	EnableRun(true);
	m_connect.SetTcpClient(this);
}

CTcpClient::~CTcpClient()
{
}

void CTcpClient::Connect( XString strIP, uint16 nPort )
{
	if (IsConnected())
	{
		m_connect.SetStop(true);
		m_connect.Wait();
	}

	if (IsConnecting())
	{
		return;
	}

	m_strRemoteIP = strIP;
	m_uRemotePort = nPort;

	SetState(EState_Connecting);
	m_connect.Start(true);
}

void CTcpClient::Release()
{
	delete this;
}

void CTcpClient::OnIOCompleteSuccess( COverLappedBuffer* pIOBuff )
{
	CIOCPSession::OnIOCompleteSuccess(pIOBuff);

	switch (pIOBuff->GetOper())
	{
	case ETCPClient_OPER_RUN:
		{
			Run();
		}
		break;
	}
}

void CTcpClient::OnIOCompleteFailed( COverLappedBuffer* pIOBuff )
{
	CIOCPSession::OnIOCompleteFailed(pIOBuff);
}

void CTcpClient::SetRelease()
{
	CIOCPSession::SetRelease();
	EnableRun(true);
}

void CTcpClient::Run()
{
	if (WillBeClose())
	{
		CloseSocket();
	}

	if (CanBeRelease())
	{
		Release();
	}
	else
	{
		PostRunIOOper();
	}
}

void CTcpClient::SetSocketOpt()
{
	SetNonBlocking(true);
	SetNoDelay(true);
	SetSendBuffSize(m_uiSendBuffSize);
	SetRecvBuffSize(m_uiRecvBuffSize);
}

void CTcpClient::OnConnected()
{
	EnableSendData(true);
	EnableRecvData(true);
	m_connect.SetStop(true);
}

void CTcpClient::EnableRun( bool bEnable )
{
	if (bEnable && !m_bEnableRun)
	{
		m_bEnableRun = bEnable;
		PostRunIOOper();
	}
	else
	{
		m_bEnableRun = bEnable;
	}
}

bool CTcpClient::PostRunIOOper()
{
	if (!m_bEnableRun)
	{
		m_runIOBuff.SetUsed(false);
		return false;
	}

	m_runIOBuff.SetUsed(true);
	DWORD dwNumBytes = 0;
	m_runIOBuff.SetUsedLength(m_runIOBuff.GetMaxBuffSize());
	m_runIOBuff.SetOper(ETCPClient_OPER_RUN);

	if (m_idleRunThread.GetInstance().GetIOCP().PostStatus(reinterpret_cast<ULONG_PTR>(this), dwNumBytes, static_cast<OVERLAPPED*>(&m_runIOBuff)))
	{
		return true;
	}

	//OnError();
	Close();
	m_runIOBuff.SetUsed(false);
	return false;
}

bool CTcpClient::Connecting()
{
	SetNonBlocking(false);
	bool bRet = CTcpSocket::Connect(m_strRemoteIP, m_uRemotePort);

	if (bRet)
	{
		SetSocketOpt();
		if (m_IOWorkThread.GetInstance().GetIOCP().AssociateDevice(reinterpret_cast<HANDLE>(GetSocket()), reinterpret_cast<ULONG_PTR>(this)))
		{
			SetBindIOCP(&m_IOWorkThread.GetInstance().GetIOCP());
			//Core::LOG("Associated connection socket");
			SetState(EState_Connected);
			OnConnected();
		}
		else
		{
			Core::LOG("Associated connection failed");
		}
	}

	return bRet;
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)