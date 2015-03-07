#ifndef CORE_UTILITY_NETWORK_SOCKET_H__INCLUDED
#define CORE_UTILITY_NETWORK_SOCKET_H__INCLUDED

#include "Network/SockAddr.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CSocket
{
public:
	CSocket();
	CSocket(SOCKET s);
	virtual ~CSocket();

	virtual bool Create();
	virtual void Close();
	virtual bool Bind(uint16 port, const XString& strIP = TEXT(""));
	const CSockAddr& GetSockAddrBind();
	virtual void Attach(SOCKET s);
	virtual SOCKET Detach();
	SOCKET GetSocket() const;
	
	bool SetNonBlocking(bool bNonBlocking);
	bool SetNoDelay(bool bNoDelay);
	bool SetKeepAlive(bool bKeepLive, uint32 uiKeepAliveInterval = 10000, uint32 uiKeeyAliveTime = 3000);
	bool SetLinger(bool bLinger, uint32 uiTimeOut = 6000);
	bool SetSendBuffSize(uint32 uiLength);
	bool SetRecvBuffSize(uint32 uiLength);

	virtual void OnError(int32 iErrCode, const XString& strDescription = TEXT(""));

protected:
	SOCKET CreateSocket(uint32 uiType, XString strProtocol = TEXT(""));
	virtual void OnClose();

	static uint32 GetSockOperErr();

	SOCKET m_sock;
	CSockAddr m_bindAddr;

	static WSAInitializer s_sockInit;

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif