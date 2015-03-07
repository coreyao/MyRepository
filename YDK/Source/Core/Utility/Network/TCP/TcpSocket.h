#ifndef CORE_UTILITY_NETWORK_TCPSOCKET_H__INCLUDED
#define CORE_UTILITY_NETWORK_TCPSOCKET_H__INCLUDED

#include "Network/Socket.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CTcpSocket : public CSocket
{
public:
	enum EState
	{
		EState_Close = 0,
		EState_Connecting,
		EState_Connected,
		EState_Listening
	};

	CTcpSocket();
	virtual ~CTcpSocket();

	virtual bool Create();
	virtual void Close();
	virtual void Attach(SOCKET s, EState st);
	virtual SOCKET Detach();

	virtual bool Listen(uint32 uiBackLog = 5);
	virtual bool Accept(SOCKET& s);
	virtual bool Accept(CTcpSocket& s);
	virtual bool Connect(const XString& strIP, uint16 port);
	virtual void CheckConnection();

	int32 SendData(const XChar* pData, uint32 uiDataLength);
	int32 RecvData(XChar* pBuff, uint32 uiBufferLength);

	const CSockAddr& GetRemoteAddr() const;

	EState GetState() const;

	bool IsClosed() const;
	bool IsConnecting() const;
	bool IsConnected() const;
	bool IsListening() const;

	virtual void OnError(int32 iErrCode, const XString& strDescription /* = TEXT */);

protected:
	void SetState(EState st);
	virtual void OnAccept();
	virtual void OnDataSendComplete();
	virtual void OnConnected();

	CSockAddr m_remoteAddr;
	EState m_state;

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif