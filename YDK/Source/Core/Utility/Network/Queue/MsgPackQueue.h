#ifndef CORE_UTILITY_NETWORK_QUEUE_MSGPACKQUEUE_H__INCLUDED
#define CORE_UTILITY_NETWORK_QUEUE_MSGPACKQUEUE_H__INCLUDED

#include <queue>
#include "BasicDefines.h"
#include "UtilityTypes.h"
#include "Network/Packet/Msg.h"
#include "Lockers/Locker.h"

using Core::Utility::CLocker;

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

struct SMsgPack
{
	SMsgPack()
		: m_uLength(0)
		, m_bGot(false)
		, m_pMsg(NULL)
		, m_uPushedLength(0)
		, m_uPopedLength(0)
	{
	}

	uint32 m_uPushedLength;
	uint32 m_uPopedLength;
	uint32 m_uLength;
	bool m_bGot;
	SMsg* m_pMsg;
};


class CMsgPackQueue
{
public:
	CMsgPackQueue();
	~CMsgPackQueue();

	bool PushData(char* pData, uint32& uLength);
	bool PushMsgPack(SMsg* msg, uint32 uPushDataLength = 0);
	bool PopMsgPack(uint32 uPopDataLength = 0);
	void Clear();
	SMsgPack* GetFront();
	SMsgPack* GetBack();
	uint32 GetSize() const;

private:
	std::queue<SMsgPack*> m_queue;
	bool m_bIsIntegrity;
	CLocker m_lock;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif