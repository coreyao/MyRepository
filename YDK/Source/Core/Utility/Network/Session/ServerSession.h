#ifndef CORE_UTILITY_NETWORK_SESSION_SERVERSISSION_H__INCLUDED
#define CORE_UTILITY_NETWORK_SESSION_SERVERSISSION_H__INCLUDED

#include "Network/Packet/Msg.h"
#include "Network/Queue/MsgPackQueue.h"
#include "Network/Session/IOCPSession.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CServerSession: public CIOCPSession
{
public:
	CServerSession();
	virtual ~CServerSession();

	bool SendMsgPack(const SMsg* msg);
	SMsg* PopMsgPack();

protected:
	virtual bool OnGetData(char* pData, uint32 uiLength);
	virtual void OnPrepareSendData();

	CMsgPackQueue m_sendQueue;
	CMsgPackQueue m_recvQueue;

	COverLappedBuffer m_tempSendBuff;
	COverLappedBuffer m_tempRecvBuff;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif