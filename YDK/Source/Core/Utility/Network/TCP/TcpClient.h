#ifndef CORE_UTILITY_NETWORK_TCPCLIENT_H__INCLUDED
#define CORE_UTILITY_NETWORK_TCPCLIENT_H__INCLUDED

#include "Network/Session/IOCPSession.h"
#include "Network/iocp/IOCPSessionWorkThread.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CTcpClient;
class CTcpClientConnectionThread : public XThread
{
public:
	CTcpClientConnectionThread();
	~CTcpClientConnectionThread();

	virtual void Run();

	void SetTcpClient(CTcpClient* val) { m_tcpClient = val; }

private:
	CTcpClient* m_tcpClient;
	
};


class CTcpClient : public CIOCPSession
{
protected:
	enum ETCPClient_OPER
	{
		ETCPClient_OPER_BEGIN = EIOCPSESSION_OPER_END,
		ETCPClient_OPER_RUN,
		ETCPClient_OPER_END
	};

public:
	CTcpClient();
	virtual ~CTcpClient();

	virtual void Connect(XString strIP, uint16 nPort);
	virtual void Release();
	virtual void OnIOCompleteSuccess(COverLappedBuffer* pIOBuff);
	virtual void OnIOCompleteFailed(COverLappedBuffer* pIOBuff);

	virtual void SetRelease();
	bool Connecting();

protected:
	virtual void Run();
	virtual void SetSocketOpt();
	virtual void OnConnected();

	void EnableRun(bool bEnable);
	bool PostRunIOOper();

protected:
	COverLappedBuffer m_runIOBuff;
	TSingleton<CIOCPSessionWorkThread> m_IOWorkThread;
	bool m_bEnableRun;

	uint32 m_uiSendBuffSize;
	uint32 m_uiRecvBuffSize;

	XString m_strRemoteIP;
	uint16 m_uRemotePort;

	CTcpClientConnectionThread m_connect;

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif