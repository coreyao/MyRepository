#include "Network/iocp/IOCPSession.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

IOCPSession::IOCPSession()
: m_sendIOBuff(SO_MAX_MSG_SIZE)
, m_recvIOBuff(SO_MAX_MSG_SIZE)
, m_bakIOBuff(SO_MAX_MSG_SIZE)
, m_bWillBeRelease(false)
, m_uiProcessedLength(0)
, m_uiTempRecvdLength(0)
, m_bEnableSend(false)
, m_bEnableRecv(false)
, m_bWillBeClose(false)
{
}

IOCPSession::IOCPSession(uint32 uiSendRecvBuff)
: m_sendIOBuff(uiSendRecvBuff)
, m_recvIOBuff(uiSendRecvBuff)
, m_bakIOBuff(uiSendRecvBuff)
, m_bWillBeRelease(false)
, m_uiProcessedLength(0)
, m_uiTempRecvdLength(0)
, m_bEnableSend(false)
, m_bEnableRecv(false)
, m_bWillBeClose(false)
{
}

IOCPSession::~IOCPSession()
{
}

void IOCPSession::Close()
{
	EnableRecvData(false);
	EnableSendData(false);
	m_bWillBeClose = true;
}

Core::Utility::uint32 IOCPSession::PushSendData( const char* pData, uint32 uiLength )
{
	//Lock here
	return m_bakIOBuff.PushData(pData, uiLength);
}

Core::Utility::uint32 IOCPSession::GetEmptyPushBuffSize() const
{
	return m_bakIOBuff.GetMaxBuffSize() - m_bakIOBuff.GetUsedLength();
}

bool IOCPSession::GetRecvData( char* pData, uint32& uiLength )
{
	return true;
}

bool IOCPSession::SendSessionData()
{
	if (!m_bEnableSend || !IsConnected() || m_bWillBeClose)
	{
		return false;
	}

	if (m_sendIOBuff.IsEmpty() && m_bakIOBuff.IsEmpty())
	{
		return false;
	}

	if (m_sendIOBuff.IsEmpty())
	{
		//lock here
		m_sendIOBuff.SwapData(m_bakIOBuff);
	}

	DWORD dwNumBytes = 0;
	m_sendIOBuff.SetOper(EIOCPSESSION_OPER_SEND_COMPLETE);

	int32 nRet = WSASend(GetSocket(), m_sendIOBuff.GetWSABuff(), 1, &dwNumBytes, 0, static_cast<LPWSAOVERLAPPED>(&m_sendIOBuff), NULL);
	if (nRet == SOCKET_ERROR)
	{
		int32 nErr = GetSockOperErr();
		if (nErr != ERROR_IO_PENDING)
		{
			//OnError();
			Close();

			return false;
		}
	}

	return true;
}

bool IOCPSession::RecvSessionData()
{
	if (!m_bEnableRecv || !IsConnected() || m_bWillBeRelease)
	{
		return false;
	}

	if (m_uiProcessedLength == m_uiTempRecvdLength)
	{
		m_recvIOBuff.Clear();
		m_recvIOBuff.SetUsedLength(m_recvIOBuff.GetMaxBuffSize());
		m_uiProcessedLength = 0;
		m_uiTempRecvdLength = 0;
	}
	else
	{
		m_recvIOBuff.PopData(m_recvIOBuff.GetNumberOfBytes());
	}

	m_recvIOBuff.SetNumberOfBytes(0);
	if (m_recvIOBuff.IsEmpty())
	{
		return false;
	}

	DWORD dwNumBytes = 0;
	DWORD dwFlags = 0;
	m_recvIOBuff.SetOper(EIOCPSESSION_OPER_RECV_COMPLETE);

	int32 nRet = WSARecv(GetSocket(), m_recvIOBuff.GetWSABuff(), 1, &dwNumBytes, &dwFlags, static_cast<LPWSAOVERLAPPED>(&m_recvIOBuff), NULL);
	if (nRet == SOCKET_ERROR)
	{
		int32 nErr = GetSockOperErr();
		if (nErr != ERROR_IO_PENDING)
		{
			//OnError();
			Close();

			return false;
		}
	}

	return true;
}

void IOCPSession::EnableSendData( bool bEnable )
{
	if (bEnable && !m_bEnableSend)
	{
		m_bEnableSend = bEnable;
		PostSendIOOper();
	}
	else
	{
		m_bEnableSend = bEnable;
	}
}

void IOCPSession::EnableRecvData( bool bEnable )
{
	if (bEnable && !m_bEnableRecv)
	{
		m_bEnableRecv = bEnable;
		PostRecvIOOper();
	}
	else
	{
		m_bEnableRecv = bEnable;
	}
}

void IOCPSession::SetProcessedLength( uint32 uiLength )
{
	m_uiProcessedLength = uiLength;
}

Core::Utility::uint32 IOCPSession::GetProcessedLength() const
{
	return m_uiProcessedLength;
}

bool IOCPSession::OnGetData( char* pData, uint32 uiLength )
{
	return true;
}

void IOCPSession::OnPrepareSendData()
{
}

void IOCPSession::OnIOCompleteSuccess( OverLappedBuffer* pIOBuff )
{
	if (pIOBuff == NULL)
	{
		return;
	}

	switch (pIOBuff->GetOper())
	{
	case EIOCPSESSION_OPER_SEND_COMPLETE:
		{
			if (pIOBuff->GetNumberOfBytes() == 0)
			{
				//OnError();
				Close();
				pIOBuff->SetUsed(false);
			}

			if (pIOBuff->GetUsedLength() < pIOBuff->GetNumberOfBytes())
			{
				//OnError();
				Close();
				pIOBuff->SetUsed(false);
				return;
			}
			else
			{
				pIOBuff->PopData(pIOBuff->GetNumberOfBytes());
			}

			OnPrepareSendData();
			if (!SendSessionData())
			{
				PostSendIOOper(false);
			}
		}
		break;
	case EIOCPSESSION_OPER_SEND:
		{
			pIOBuff->Clear();
			OnPrepareSendData();
			if (!SendSessionData())
			{
				PostSendIOOper(false);
			}
		}
		break;
	case EIOCPSESSION_OPER_RECV_COMPLETE:
		{
			if (pIOBuff->GetNumberOfBytes() == 0)
			{
				//OnError();
				Close();
				pIOBuff->SetUsed(false);
				return;
			}

			m_uiTempRecvdLength += pIOBuff->GetNumberOfBytes();
			bool bProcessed = OnGetData(const_cast<char*>(pIOBuff->GetBuffer()), pIOBuff->GetNumberOfBytes());
			if (bProcessed)
			{
				m_uiProcessedLength += pIOBuff->GetNumberOfBytes();
			}

			if (!RecvSessionData())
			{
				PostRecvIOOper(false);
			}
		}
		break;
	case EIOCPSESSION_OPER_RECV:
		{
			if (!RecvSessionData())
			{
				PostRecvIOOper(false);
			}
		}
		break;
	default:
		break;
	}
}

void IOCPSession::OnIOCompleteFailed( OverLappedBuffer* pIOBuff )
{
	//OnError();
	Close();
	pIOBuff->SetUsed(false);
}

void IOCPSession::SetRelease()
{
	Close();
	m_bWillBeRelease = true;
}

bool IOCPSession::WillBeRelease() const
{
	return m_bWillBeRelease;
}

bool IOCPSession::CanBeRelease() const
{
	return (m_bWillBeRelease && !HaveUncompletionOper());
}

bool IOCPSession::WillBeClose() const
{
	return m_bWillBeClose;
}

void IOCPSession::CloseSocket()
{
	TcpSocket::Close();
	m_bWillBeClose = false;
}

bool IOCPSession::HaveUncompletionOper() const
{
	return m_sendIOBuff.IsUsed() || m_recvIOBuff.IsUsed();
}

bool IOCPSession::PostRecvIOOper( bool bCheckIOBuffUsed /*= true*/ )
{
	if (bCheckIOBuffUsed && m_recvIOBuff.IsUsed())
	{
		return false;
	}

	if (m_bWillBeRelease)
	{
		m_recvIOBuff.SetUsed(false);
		return false;
	}

	if (!m_bEnableRecv)
	{
		m_recvIOBuff.SetUsed(false);
		return false;
	}

	if (IsConnecting())
	{
		return false;
	}

	m_recvIOBuff.SetUsed(true);
	DWORD dwNumBytes = 0;
	m_recvIOBuff.SetUsedLength(m_recvIOBuff.GetMaxBuffSize());
	m_recvIOBuff.SetOper(EIOCPSESSION_OPER_RECV);

	if (m_idleRunThread.GetInstance().GetIOCP().PostStatus(reinterpret_cast<ULONG_PTR>(this), dwNumBytes, static_cast<OVERLAPPED*>(&m_recvIOBuff)))
	{
		return true;
	}

	//OnError();
	Close();
	m_recvIOBuff.SetUsed(false);
	return false;
}

bool IOCPSession::PostSendIOOper( bool bCheckIOBuffUsed /*= true*/ )
{
	if (bCheckIOBuffUsed && m_sendIOBuff.IsUsed())
	{
		return false;
	}

	if (m_bWillBeRelease)
	{
		m_sendIOBuff.SetUsed(false);
		return false;
	}

	if (!m_bEnableRecv)
	{
		m_sendIOBuff.SetUsed(false);
		return false;
	}

	if (IsConnecting())
	{
		return false;
	}

	m_sendIOBuff.SetUsed(true);
	DWORD dwNumBytes = 0;
	m_sendIOBuff.SetUsedLength(m_sendIOBuff.GetMaxBuffSize());
	m_sendIOBuff.SetOper(EIOCPSESSION_OPER_RECV);

	if (m_idleRunThread.GetInstance().GetIOCP().PostStatus(reinterpret_cast<ULONG_PTR>(this), dwNumBytes, static_cast<OVERLAPPED*>(&m_sendIOBuff)))
	{
		return true;
	}

	//OnError();
	Close();
	m_sendIOBuff.SetUsed(false);
	return false;
}
END_NAMESPACE(Utility)
END_NAMESPACE(Core)