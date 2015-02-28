#include <vector>
#include <iostream>

#include "Network/Session/IOCPSession.h"
#include "Network/iocp/IOCPSessionIdleRunThread.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CIOCPIdleRunThread::CIOCPIdleRunThread()
: m_idleRunThread(NULL)
{
}

CIOCPIdleRunThread::~CIOCPIdleRunThread()
{
}

void CIOCPIdleRunThread::Run()
{
	if (m_idleRunThread == NULL)
	{
		return;
	}

	struct IOOPER
	{
		CIOCPSession* pSession;
		COverLappedBuffer* pBuff;
	};

	std::vector<IOOPER> successIOOper;
	std::vector<IOOPER> failedIOOper;

	while (true)
	{
		ULONG_PTR pCompletionKey = NULL;
		DWORD dwNumBytes = 0;
		LPOVERLAPPED pOverlapped = NULL;
		DWORD dwMilliseconds = 0;

		if (m_idleRunThread->GetIOCP().GetStatus(&pCompletionKey, &dwNumBytes, &pOverlapped, dwMilliseconds))
		{
			CIOCPSession* pSession = reinterpret_cast<CIOCPSession*>(pCompletionKey);
			if (pSession != NULL)
			{
				COverLappedBuffer* pBuff = static_cast<COverLappedBuffer*>(pOverlapped);
				if (pBuff != NULL)
				{
					pBuff->SetNumberOfBytes(dwNumBytes);
					IOOPER oper = {0};
					oper.pSession = pSession;
					oper.pBuff = pBuff;

					successIOOper.push_back(oper);
				}
			}
		}
		else
		{
			DWORD err = GetLastError();
			if (pCompletionKey == NULL)
			{
				while (!successIOOper.empty())
				{
					IOOPER oper = successIOOper.back();
					successIOOper.pop_back();
					oper.pSession->OnIOCompleteSuccess(oper.pBuff);
				}

				while (!failedIOOper.empty())
				{
					IOOPER oper = failedIOOper.back();
					failedIOOper.pop_back();
					oper.pSession->OnIOCompleteFailed(oper.pBuff);
				}

				MSleep(1);
				break;
			}
			else
			{
				CIOCPSession* pSession = reinterpret_cast<CIOCPSession*>(pCompletionKey);
				COverLappedBuffer* pBuff = static_cast<COverLappedBuffer*>(pOverlapped);
				if (pBuff != NULL)
				{
					IOOPER oper = {0};
					oper.pSession = pSession;
					oper.pBuff = pBuff;

					failedIOOper.push_back(oper);
				}
			}
		}
	}
}

CIOCPSessionIdleRunThread::CIOCPSessionIdleRunThread()
{
	m_unIdleRunThreadCount = m_IOCP.GetIOCPThreadCount();
	StartIdleRunThread();
}

CIOCPSessionIdleRunThread::~CIOCPSessionIdleRunThread()
{
	EndIdleRunThread();
}

CIOCompletionPort& CIOCPSessionIdleRunThread::GetIOCP()
{
	return m_IOCP;
}

Core::Utility::uint32 CIOCPSessionIdleRunThread::GetIdleRunThreadNum() const
{
	return m_unIdleRunThreadCount;
}

Core::Utility::uint32 CIOCPSessionIdleRunThread::GetIOCPThreadNum() const
{
	return m_IOCP.GetIOCPThreadCount();
}

void CIOCPSessionIdleRunThread::EndIdleRunThread()
{
	for (std::list<CIOCPIdleRunThread*>::iterator itr = m_workThreads.begin();
		itr != m_workThreads.end(); 
		++itr)
	{
		CIOCPIdleRunThread* pWorkThread = *itr;
		if (pWorkThread != NULL)
		{
			pWorkThread->SetStop(true);
			pWorkThread->Wait();
			delete pWorkThread;
		}
	}
}

bool CIOCPSessionIdleRunThread::StartIdleRunThread()
{
	for (uint32 i = 0; i < m_unIdleRunThreadCount; ++i)
	{
		CIOCPIdleRunThread* pWorkThread = new CIOCPIdleRunThread;
		pWorkThread->SetIdleRunThread(this);
		pWorkThread->Start(true);
		m_workThreads.push_back(pWorkThread);
	}

	return true;
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)