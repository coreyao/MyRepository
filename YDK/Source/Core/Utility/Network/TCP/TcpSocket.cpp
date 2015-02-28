#include "Network/TCP/TcpSocket.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)


CTcpSocket::CTcpSocket()
: m_state(EState_Close)
{
}

CTcpSocket::~CTcpSocket()
{
	m_remoteAddr.Clear();
	m_state = EState_Close;
}

bool CTcpSocket::Create()
{
	if (m_sock != INVALID_SOCKET)
	{
		return false;
	}

	m_sock = CreateSocket(SOCK_STREAM, TEXT("tcp"));
	if (m_sock == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

void CTcpSocket::Close()
{
	m_remoteAddr.Clear();
	m_state = EState_Close;
	CSocket::Close();
}

void CTcpSocket::Attach(SOCKET s, Core::Utility::CTcpSocket::EState st)
{
	CSocket::Attach(s);
	m_state = st;
	if (IsConnected())
	{
		int32 length = m_remoteAddr.GetSockAddrLength();
		::getpeername(m_sock, static_cast<sockaddr*>(m_remoteAddr), &length);
	}
}

SOCKET CTcpSocket::Detach()
{
	m_remoteAddr.Clear();
	return CSocket::Detach();
}

bool CTcpSocket::Listen(Core::Utility::uint32 uiBackLog/* = 5*/)
{
	Create();
	if (::listen(m_sock, uiBackLog) == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

bool CTcpSocket::Accept(SOCKET &s)
{
	SOCKET sock = accept(m_sock, NULL, NULL);
	if (sock != INVALID_SOCKET)
	{
		s = sock;
		OnAccept();
		return true;
	}

	return false;
}

bool CTcpSocket::Accept(Core::Utility::CTcpSocket &s)
{
	int32 length = s.m_remoteAddr.GetSockAddrLength();
	SOCKET sock = accept(m_sock, static_cast<sockaddr*>(s.m_remoteAddr), &length);
	if (sock == INVALID_SOCKET)
	{
		s.Attach(sock, EState_Connected);
		OnAccept();
		return true;
	}

	return false;
}

bool CTcpSocket::Connect(const Core::Utility::XString &strIP, Core::Utility::uint16 port)
{
	m_remoteAddr.SetAddress(strIP);
	m_remoteAddr.SetAddrPort(port);
	m_remoteAddr.SetFamily(AF_INET);

	Create();
	if (::connect(m_sock, static_cast<sockaddr*>(m_remoteAddr), m_remoteAddr.GetSockAddrLength()) == SOCKET_ERROR)
	{
		int32 uiErr = GetSockOperErr();
		if (uiErr == WSAEWOULDBLOCK)
		{
			//Close();
			return true;
		}
		else
		{
			m_remoteAddr.Clear();
			//Close();
			
			return false;
		}
	}

	return true;
}

void CTcpSocket::CheckConnection()
{
	if (IsConnecting())
	{
		fd_set set = {0};
		timeval tout;

		tout.tv_sec = 10;
		tout.tv_usec = 0;
		int32 error = -1;
		int32 len = sizeof(int32);

		FD_SET(m_sock, &set);

		int32 uiRet = ::select(0, NULL, &set, NULL, &tout);
		if (uiRet > 0)
		{
			::getsockopt(m_sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &len);
			if (error == 0)
			{
				SetState(EState_Connected);
				OnConnected();
				return;
			}
		}
		else if (uiRet == 0)
		{
			Close();
		}
		else if (uiRet == SOCKET_ERROR)
		{
			Close();
			return;
		}

	}
}

int32 CTcpSocket::SendData(const Core::Utility::XChar *pData, Core::Utility::uint32 uiDataLength)
{
	int32 uiRet = ::send(m_sock, pData, uiDataLength, 0);
	if (uiRet == SOCKET_ERROR)
	{
		Close();
	}

	return uiRet;
}

int32 CTcpSocket::RecvData(XChar* pBuff, uint32 uiBufferLength)
{
	int32 uiRet = ::recv(m_sock, pBuff, uiBufferLength, 0);
	if (uiRet == SOCKET_ERROR)
	{
		Close();
	}

	return uiRet;
}

const CSockAddr& CTcpSocket::GetRemoteAddr() const
{
	return m_remoteAddr;
}

CTcpSocket::EState CTcpSocket::GetState() const
{
	return m_state;
}

bool CTcpSocket::IsClosed() const
{
	return m_state == EState_Close;
}

bool CTcpSocket::IsConnecting() const
{
	return m_state == EState_Connecting;
}

bool CTcpSocket::IsConnected() const
{
	return m_state == EState_Connected;
}

bool CTcpSocket::IsListening() const
{
	return m_state == EState_Listening;
}

void CTcpSocket::SetState( EState st )
{
	m_state = st;
}

void CTcpSocket::OnAccept()
{
}

void CTcpSocket::OnDataSendComplete()
{
}

void CTcpSocket::OnConnected()
{
}

void CTcpSocket::OnError( int32 iErrCode, const XString& strDescription /* = TEXT */ )
{
}


END_NAMESPACE(Utility)
END_NAMESPACE(Core)