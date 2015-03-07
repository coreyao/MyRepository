#include "Network/Queue/MsgPackQueue.h"
#include "Lockers/Guard.h"

using Core::Utility::CGuard;

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CMsgPackQueue::CMsgPackQueue()
: m_bIsIntegrity(true)
{
}

CMsgPackQueue::~CMsgPackQueue()
{
	Clear();
}

bool CMsgPackQueue::PushData(char* pData, uint32& uLength )
{
	if (pData != NULL && uLength != 0)
	{
		uint32 leftLength = uLength;
		while (leftLength > 0)
		{
			if (m_bIsIntegrity)
			{
				if (leftLength >= sizeof(SMsg))
				{
					SMsg* msg = reinterpret_cast<SMsg*>(pData);
					uint32 curLength = min(msg->GetLength(), leftLength);
					if (!PushMsgPack(msg, curLength))
					{
						return false;
					}

					pData += curLength;
					leftLength -= curLength;
				}
				else
				{
					break;
				}
			}
			else
			{
				SMsgPack* pack = GetBack();
				uint32 needLength = pack->m_uLength - pack->m_uPushedLength;
				if (needLength <= uLength)
				{
					memcpy(&(reinterpret_cast<char*>(pack->m_pMsg)[pack->m_uPushedLength]), pData, needLength);
					pack->m_uPushedLength += needLength;
					m_bIsIntegrity = true;

					pData += needLength;
					leftLength -= needLength;
				}
				else
				{
					memcpy(&(reinterpret_cast<char*>(pack->m_pMsg)[pack->m_uPushedLength]), pData, uLength);
					pack->m_uPushedLength += uLength;
					m_bIsIntegrity = false;

					pData += uLength;
					leftLength -= uLength;
				}
			}
		}

		uLength -= leftLength;
	}

	return true;
}

bool CMsgPackQueue::PushMsgPack(SMsg* msg, uint32 uPushDataLength /*= 0*/ )
{
	if (msg != NULL)
	{
		char* allocMsg = new char[msg->GetLength()]();
		if (uPushDataLength == 0)
		{
			uPushDataLength = msg->GetLength();
			memcpy(allocMsg, msg, msg->GetLength());
		}
		else
		{
			memcpy(allocMsg, msg, uPushDataLength);
		}

		SMsgPack* pack = new SMsgPack;
		pack->m_pMsg = reinterpret_cast<SMsg*>(allocMsg);
		pack->m_uLength = msg->GetLength();
		pack->m_bGot = false;
		pack->m_uPopedLength = 0;
		pack->m_uPushedLength = uPushDataLength;

		{
			CGuard guard(&m_lock);
			m_queue.push(pack);
		}
		
		if (pack->m_uLength > pack->m_uPushedLength)
		{
			m_bIsIntegrity = false;
		}

		return true;
	}

	return false;
}

bool CMsgPackQueue::PopMsgPack( uint32 uPopDataLength /*= 0*/ )
{
	SMsgPack* poppedMsg = GetFront();
	if (uPopDataLength == 0 || poppedMsg->m_uLength == (poppedMsg->m_uPopedLength + uPopDataLength))
	{
		{
			CGuard guard(&m_lock);
			m_queue.pop();
		}

		delete poppedMsg->m_pMsg;
		delete poppedMsg;

		return true;
	}

	if ((poppedMsg->m_uPopedLength + uPopDataLength) > poppedMsg->m_uLength)
	{
		return false;
	}

	poppedMsg->m_uPopedLength += uPopDataLength;

	return true;
}

void CMsgPackQueue::Clear()
{
	while (!m_queue.empty())
	{
		PopMsgPack();
	}
}

SMsgPack* CMsgPackQueue::GetFront()
{
	if (!m_queue.empty())
	{
		CGuard guard(&m_lock);
		return m_queue.front();
	}

	return NULL;
}

SMsgPack* CMsgPackQueue::GetBack()
{
	if (!m_queue.empty())
	{
		CGuard guard(&m_lock);
		return m_queue.back();
	}

	return NULL;
}

Core::Utility::uint32 CMsgPackQueue::GetSize() const
{
	return m_queue.size();
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
