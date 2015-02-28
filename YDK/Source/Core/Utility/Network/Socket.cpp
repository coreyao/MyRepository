#include "Network/Socket.h"
#include "LogSystem.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

WSAInitializer CSocket::s_sockInit;

CSocket::CSocket()
: m_sock(INVALID_SOCKET)
{
}

CSocket::CSocket(SOCKET s)
: m_sock(s)
{
}

CSocket::~CSocket()
{
	Close();
}

bool CSocket::Create()
{
	return true;
}

void CSocket::Close()
{
	if (m_sock != INVALID_SOCKET)
	{
		::closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		OnClose();
	}
}

bool CSocket::Bind(Core::Utility::uint16 port, const Core::Utility::XString &strIP /*= ""*/)
{
	m_bindAddr.SetAddress(strIP);
	m_bindAddr.SetAddrPort(port);

	if (::bind(m_sock, static_cast<sockaddr*>(m_bindAddr), m_bindAddr.GetSockAddrLength()) == SOCKET_ERROR)
	{
		m_bindAddr.Clear();
		Core::LOG_ERROR(XString().Format("Bind Failed: %d", WSAGetLastError()).c_str());
		return false;
	}

	return true;
}

const CSockAddr& CSocket::GetSockAddrBind()
{
	return m_bindAddr;
}

void CSocket::Attach(SOCKET s)
{
	m_sock = s;
}

SOCKET CSocket::Detach()
{
	SOCKET s = m_sock;
	m_sock = INVALID_SOCKET;
	return s;
}

SOCKET CSocket::GetSocket() const
{
	return m_sock;
}

bool CSocket::SetNonBlocking(bool bNonBlocking)
{
	u_long l = bNonBlocking ? 1 : 0;
	if (::ioctlsocket(m_sock, FIONBIO, &l) == SOCKET_ERROR)
	{
		return false;
	}
	
	return true;
}

bool CSocket::SetNoDelay(bool bNoDelay)
{
	int32 iOptVal = bNoDelay ? 1 : 0;
	int32 iRet = ::setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY,
		reinterpret_cast<const char*>(&iOptVal), sizeof(iOptVal));
	if (iRet == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

bool CSocket::SetKeepAlive(bool bKeepLive, Core::Utility::uint32 uiKeepAliveInterval/* = 10000*/, Core::Utility::uint32 uiKeeyAliveTime/* = 3000*/)
{
	return true;
}

bool CSocket::SetLinger(bool bLinger, Core::Utility::uint32 uiTimeOut/* = 6000*/)
{
	struct linger op = {0};
	op.l_onoff = bLinger ? 1 : 0;
	op.l_linger = static_cast<uint16>(uiTimeOut/1000);
	int32 nRet = ::setsockopt(m_sock, SOL_SOCKET, SO_LINGER, reinterpret_cast<const char*>(&op), sizeof(op));
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

bool CSocket::SetSendBuffSize(uint32 uiLength)
{
	int32 nRet = ::setsockopt(m_sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&uiLength), sizeof(uiLength));
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

bool CSocket::SetRecvBuffSize(uint32 uiLength)
{
	int32 nRet = ::setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&uiLength), sizeof(uiLength));
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

void CSocket::OnClose()
{

}

void CSocket::OnError(int32 iErrCode, const XString& strDescription /* = TEXT */)
{

}

SOCKET CSocket::CreateSocket(Core::Utility::uint32 uiType, Core::Utility::XString strProtocol/* = ""*/)
{
	struct protoent* p = NULL;
	int32 protono = 0;

	if (!strProtocol.empty())
	{
		strProtocol.ToLower();
		if (strProtocol == TEXT("tcp"))
		{
			protono = IPPROTO_TCP;
		}
		else if (strProtocol == TEXT("udp"))
		{
			protono = IPPROTO_UDP;
		}
		else
		{
			p = ::getprotobyname(strProtocol.c_str());
			if (p == NULL)
			{
				return INVALID_SOCKET;
			}
			else
			{
				protono = p->p_proto;
			}
		}
	}

	return ::socket(AF_INET, uiType, protono);
}

uint32 CSocket::GetSockOperErr()
{
	return ::WSAGetLastError();
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
