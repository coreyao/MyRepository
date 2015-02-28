#ifndef CORE_UTILITY_NETWORK_TCPSERVER_H__INCLUDED
#define CORE_UTILITY_NETWORK_TCPSERVER_H__INCLUDED

#include <set>
#include "BasicDefines.h"
#include "UtilityTypes.h"
#include "Network/tcp/TcpSocket.h"
#include "XThread/XThread.h"
#include "Network/Session/IOCPSession.h"
#include "Network/iocp/IOCPSessionWorkThread.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CTcpServer;
class CTcpServerRunThread : public XThread
{
public:
	CTcpServerRunThread();
	~CTcpServerRunThread();

	virtual void Run();

	void SetServer(CTcpServer* val) { m_server = val; }

private:
	CTcpServer* m_server;
	
};

class CTcpServer
{
	class ListenSocket : public CTcpSocket
	{
	public:
		ListenSocket() : m_pServer(NULL){}

		CTcpServer* m_pServer;

	protected:
		virtual void OnError(int32 iErrCode, const XChar* strDescription = NULL);

	};

public:
	CTcpServer();
	virtual ~CTcpServer();

	virtual void Init(uint16 uPort, uint32 uMaxConnect, XString strBindLocalIP = TEXT(""), uint32 uiSocketBuff = 1*1024*1024);
	virtual bool ListenSocketOpt();
	virtual bool Start(bool bNewThread = true, int32 nThreadPriority = THREAD_PRIORITY_HIGHEST);
	virtual void End();
	virtual bool StartListen();
	virtual void StopListen();
	virtual void Run();
	virtual void RunOneStep();

protected:
	virtual CIOCPSession* CreateIOCPSession();

	virtual void ReleaseSession(CIOCPSession* pIOCPSession);
	virtual void OnAcceptNewSession(CIOCPSession* pIOCPSession);
	virtual void OnRunOneStep();
	virtual void AcceptProcess();
	virtual void CloseSessionProcess();
	virtual void ReleaseSessionProcess();
	virtual void ReleaseAllSession();

protected:
	virtual void OnError(int32 nError, const XChar* pErrDes = NULL);

protected:
	std::set<CIOCPSession*> m_sessionList;
	std::set<CIOCPSession*> m_willReleaseList;

	ListenSocket m_listen;
	CTcpServerRunThread m_runThread;
	uint32 m_uiMaxConnect;
	uint32 m_uiSocketBuffSize;

	uint16 m_uiPort;
	XString m_strLocalIP;

	bool m_isRunning;

	TSingleton<CIOCPSessionWorkThread> m_IOWorkThread;

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif