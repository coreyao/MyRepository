#include "Network/UDP/UdpSocket.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CUdpSocket::CUdpSocket()
{
}

CUdpSocket::~CUdpSocket()
{
}

bool CUdpSocket::Create()
{
	if (m_sock != INVALID_SOCKET)
	{
		return false;
	}

	m_sock = CreateSocket(SOCK_DGRAM, TEXT("udp"));
	if (m_sock == INVALID_SOCKET)
	{
		return false;
	}
	return true;
}

Core::Utility::int32 CUdpSocket::SendTo(CSockAddr& addr, const char* pBuff, uint32 uiLength, int32 iFlag /*= 0*/ )
{
	int32 uiRet = ::sendto(m_sock, pBuff, uiLength, iFlag, reinterpret_cast<sockaddr*>(&addr), addr.GetSockAddrLength());
	if (uiRet == SOCKET_ERROR)
	{
	}

	return uiRet;
}

Core::Utility::int32 CUdpSocket::RecvFrom( CSockAddr& addr, char* pBuff, uint32 uiLength, int32 uiFlag /*= 0*/ )
{
	int32 len = addr.GetSockAddrLength();
	int32 uiRet = ::recvfrom(m_sock, pBuff, uiLength, uiFlag, reinterpret_cast<sockaddr*>(&addr), &len);
	if (uiRet == SOCKET_ERROR)
	{
	}

	return uiRet;
}


END_NAMESPACE(Utility)
END_NAMESPACE(Core)