#include "Network/Session/ServerSession.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CServerSession::CServerSession()
{
}

CServerSession::~CServerSession()
{
}

bool CServerSession::SendMsgPack( const SMsg* msg )
{
	if (msg != NULL)
	{
		m_sendQueue.PushMsgPack(const_cast<SMsg*>(msg));
		return true;
	}
	
	return false;
}

SMsg* CServerSession::PopMsgPack()
{
	while (true)
	{
		SMsgPack* poppedMsg = m_recvQueue.GetFront();
		if (poppedMsg == NULL)
		{
			break;
		}
		else
		{
			if (poppedMsg->m_bGot)
			{
				m_recvQueue.PopMsgPack();
				continue;
			}
			else
			{
				if (poppedMsg->m_uLength == poppedMsg->m_uPushedLength)
				{
					poppedMsg->m_bGot = true;
					return poppedMsg->m_pMsg;
				}
				else
				{
					break;
				}
			}
		}
	}

	return NULL;
}

bool CServerSession::OnGetData( char* pData, uint32 uiLength )
{
	if (m_tempRecvBuff.IsEmpty())
	{
		uint32 totalLength = uiLength;
		if (!m_recvQueue.PushData(pData, uiLength))
		{
			return false;
		}

		if (uiLength < totalLength)
		{
			m_tempRecvBuff.PushData(&pData[uiLength], totalLength - uiLength);
		}
	}
	else
	{
		uint32 freeSize = m_tempRecvBuff.GetMaxBuffSize() - m_tempRecvBuff.GetUsedLength();
		uint32 pushLength = min(freeSize, uiLength);
		m_tempRecvBuff.PushData(pData, pushLength);
		uint32 tempLength = m_tempRecvBuff.GetUsedLength();
		if (!m_recvQueue.PushData(m_tempRecvBuff.GetBuffer(), tempLength))
		{
			return false;
		}

		m_tempRecvBuff.PopData(tempLength);

		if (freeSize < uiLength)
		{
			uint32 leftLength = uiLength - freeSize;
			m_tempRecvBuff.PushData(&pData[freeSize], leftLength);
			tempLength = leftLength;
			if (!m_recvQueue.PushData(m_tempRecvBuff.GetBuffer(), tempLength))
			{
				return false;
			}

			m_tempRecvBuff.PopData(tempLength);
		}
	}

	return true;
}

void CServerSession::OnPrepareSendData()
{
	while (true)
	{
		if (!m_tempSendBuff.IsEmpty())
		{
			uint32 pushLength = PushSendData(reinterpret_cast<char*>(m_tempSendBuff.GetBuffer()), m_tempSendBuff.GetUsedLength());
			m_tempSendBuff.PopData(pushLength);

			if (!m_tempSendBuff.IsEmpty())
			{
				return;
			}
		}
		else
		{
			while (true)
			{
				SMsgPack* poppedMsg = m_sendQueue.GetFront();
				if (poppedMsg != NULL && poppedMsg->m_uPushedLength == poppedMsg->m_uLength)
				{
					uint32 curLength = poppedMsg->m_uLength - poppedMsg->m_uPopedLength;
					uint32 leftLength = m_tempSendBuff.GetMaxBuffSize() - m_tempSendBuff.GetUsedLength();
					uint32 tempLength = min(curLength, leftLength);
					if (leftLength < sizeof(SMsg))
					{
						break;
					}
					else
					{
						m_tempSendBuff.PushData(&(reinterpret_cast<char*>(poppedMsg->m_pMsg)[poppedMsg->m_uPopedLength]), tempLength);
						m_sendQueue.PopMsgPack(tempLength);
					}
				}
				else
				{
					break;
				}
			}

			if (m_tempSendBuff.IsEmpty())
			{
				return;
			}
		}
	}
}
END_NAMESPACE(Utility)
END_NAMESPACE(Core)