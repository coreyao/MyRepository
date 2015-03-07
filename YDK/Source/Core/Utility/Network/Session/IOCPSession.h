#ifndef CORE_UTILITY_NETWORK_IOCP_IOCPSESSION_H__INCLUDED
#define CORE_UTILITY_NETWORK_IOCP_IOCPSESSION_H__INCLUDED

#include "Singleton.h"
#include "Lockers/Locker.h"
#include "Network/tcp/TcpSocket.h"
#include "Network/iocp/IOCPObject.h"
#include "Network/iocp/IOCPSessionIdleRunThread.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

using Core::Utility::CLocker;

class CIOCPSession : public CTcpSocket, public CIOCPObject
{
protected:
	enum EIOCPSESSION_OPER
	{
		EIOCPSESSION_OPER_BEGIN = 0,
		EIOCPSESSION_OPER_SEND,
		EIOCPSESSION_OPER_SEND_COMPLETE,
		EIOCPSESSION_OPER_RECV,
		EIOCPSESSION_OPER_RECV_COMPLETE,
		EIOCPSESSION_OPER_END
	};

public:
	CIOCPSession();
	CIOCPSession(uint32 uiSendRecvBuff);
	virtual ~CIOCPSession();

	virtual void Close();

	uint32 PushSendData(const char* pData, uint32 uiLength);
	uint32 GetEmptyPushBuffSize() const;

	bool GetRecvData(char* pData, uint32& uiLength);

	bool SendSessionData();
	bool RecvSessionData();

	void EnableSendData(bool bEnable);
	void EnableRecvData(bool bEnable);

	void SetProcessedLength(uint32 uiLength);
	uint32 GetProcessedLength() const;

	virtual bool OnGetData(char* pData, uint32 uiLength);
	virtual void OnPrepareSendData();

	virtual void OnIOCompleteSuccess(COverLappedBuffer* pIOBuff);
	virtual void OnIOCompleteFailed(COverLappedBuffer* pIOBuff);

	virtual void SetRelease();
	virtual bool WillBeRelease() const;
	virtual bool CanBeRelease() const;

	virtual bool WillBeClose() const;
	virtual void CloseSocket();

	virtual bool HaveUncompletionOper() const;

protected:
	bool PostRecvIOOper(bool bCheckIOBuffUsed = true);
	bool PostSendIOOper(bool bCheckIOBuffUsed = true);

	COverLappedBuffer m_sendIOBuff;
	COverLappedBuffer m_recvIOBuff;
	COverLappedBuffer m_bakIOBuff;

	bool m_bWillBeRelease;
	bool m_bEnableSend;
	bool m_bEnableRecv;

	uint32 m_uiProcessedLength;
	uint32 m_uiTempRecvdLength;

	bool m_bWillBeClose;

	CLocker m_lock;

protected:
	TSingleton<CIOCPSessionIdleRunThread> m_idleRunThread;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif