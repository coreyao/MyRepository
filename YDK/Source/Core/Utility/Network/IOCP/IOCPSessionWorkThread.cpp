#include "Network/Session/IOCPSession.h"
#include "Network/iocp/IOCPSessionWorkThread.h"
#include "LogSystem.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CIOCPWorkThread::CIOCPWorkThread()
: m_workThread(NULL)
{
}

CIOCPWorkThread::~CIOCPWorkThread()
{
}

void CIOCPWorkThread::Run()
{
	if (m_workThread == NULL)
	{
		return;
	}

	ULONG_PTR pCompletionKey = NULL;
	DWORD dwNumBytes = 0;
	LPOVERLAPPED pOverlapped = NULL;
	DWORD dwMilliseconds = 0;

	if (m_workThread->GetIOCP().GetStatus(&pCompletionKey, &dwNumBytes, &pOverlapped, dwMilliseconds))
	{
		CIOCPSession* pSession = reinterpret_cast<CIOCPSession*>(pCompletionKey);
		if (pSession != NULL)
		{
			COverLappedBuffer* pBuff = static_cast<COverLappedBuffer*>(pOverlapped);
			if (pBuff != NULL)
			{
				//Core::LOG(XString().Format("oper=%d", pBuff->GetOper()).c_str());
				pBuff->SetNumberOfBytes(dwNumBytes);
				pSession->OnIOCompleteSuccess(pBuff);
			}
			else
			{
				Core::LOG(XString().Format("buff is null").c_str());
			}
		}
		else
		{
			Core::LOG(XString().Format("session is null").c_str());
		}
	}
	else
	{
		DWORD err = GetLastError();
		if (err != 258)
		{
			Core::LOG(XString().Format("err=%d", err).c_str());
		}
		if (pCompletionKey == NULL)
		{
			MSleep(1);
		}
		else
		{
			CIOCPSession* pSession = reinterpret_cast<CIOCPSession*>(pCompletionKey);
			COverLappedBuffer* pBuff = static_cast<COverLappedBuffer*>(pOverlapped);
			if (pBuff != NULL)
			{
				pSession->OnIOCompleteFailed(pBuff);
			}
		}
	}
}

CIOCPSessionWorkThread::CIOCPSessionWorkThread()
{
	m_unWorkThreadCount = m_IOCP.GetIOCPThreadCount();
	StartWorkThread();
}

CIOCPSessionWorkThread::~CIOCPSessionWorkThread()
{
	EndWorkThread();
}

CIOCompletionPort& CIOCPSessionWorkThread::GetIOCP()
{
	return m_IOCP;
}

uint32 CIOCPSessionWorkThread::GetWorkThreadNum() const
{
	return m_unWorkThreadCount;
}

uint32 CIOCPSessionWorkThread::GetIOCPThreadNum() const
{
	return m_IOCP.GetIOCPThreadCount();
}

bool CIOCPSessionWorkThread::StartWorkThread()
{
	for (uint32 i = 0; i < m_unWorkThreadCount; ++i)
	{
		CIOCPWorkThread* pWorkThread = new CIOCPWorkThread;
		pWorkThread->SetWorkThread(this);
		pWorkThread->Start(true);
		m_workThreads.push_back(pWorkThread);
	}

	return true;
}

void CIOCPSessionWorkThread::EndWorkThread()
{
	for (std::list<CIOCPWorkThread*>::iterator itr = m_workThreads.begin();
		itr != m_workThreads.end(); 
		++itr)
	{
		CIOCPWorkThread* pWorkThread = *itr;
		if (pWorkThread != NULL)
		{
			pWorkThread->SetStop(true);
			pWorkThread->Wait();
			delete pWorkThread;
		}
	}
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)