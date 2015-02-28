#include <ctime>
#include "Network/IOCP/IOCPSessionWorkThread.h"
#include "Network/IOCP/IOCPSessionIdleRunThread.h"
#include "Network/ThreadServer.h"
#include "Singleton.h"


BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CLogicThread::CLogicThread()
: m_threadServer(NULL)
{
}

CLogicThread::~CLogicThread()
{
}

void CLogicThread::Run()
{
	if (m_threadServer != NULL)
	{
		m_threadServer->Process();
		m_threadServer->SetLastProcLogicTime(::time(NULL));
	}

	MSleep(1);
}

LogicTimeoutThread::LogicTimeoutThread()
: m_threadServer(NULL)
{
}

LogicTimeoutThread::~LogicTimeoutThread()
{
}

void LogicTimeoutThread::Run()
{
	if (m_threadServer != NULL && m_threadServer->GetOpenTimeoutDump())
	{
		uint64 nowTime = ::time(NULL);
		if (nowTime > m_threadServer->GetLastProcLogicTime()
		&& nowTime - m_threadServer->GetLastProcLogicTime() > m_threadServer->GetTimeoutValue())
		{
			m_threadServer->Dump();
			m_threadServer->SetLastProcLogicTime(::time(NULL));
		}
	}

	MSleep(1);
}

CThreadServer::CThreadServer()
: m_bOpenTimeoutDump(false)
, m_lastProcLogicTime(::time(NULL))
, m_uTimeoutValue(10 * 1000)
, m_eDumpType(MiniDumpNormal)
{
	m_logicThread.SetThreadServer(this);
	m_logicTimeoutThread.SetThreadServer(this);
}

CThreadServer::~CThreadServer()
{
	ExitServer();
}

void CThreadServer::Startup( bool bConsole /*= true*/, bool bOpenTimeoutDump /*= false*/, uint32 uTimeoutValue /*= 10000*/, MINIDUMP_TYPE eDumpType /*= MiniDumpNormal*/ )
{
	m_bOpenTimeoutDump = bOpenTimeoutDump;
	m_uTimeoutValue = uTimeoutValue;
	m_eDumpType = eDumpType;

	StartLogicThread();

	if (bOpenTimeoutDump)
	{
		StartTimeoutDumpThread();
	}

	if (bConsole)
	{
		ProcessCommand();
	}
}

void CThreadServer::Shutdown()
{
	ExitServer();
}

void CThreadServer::StartLogicThread()
{
	m_logicThread.Start(true);
}

void CThreadServer::StartTimeoutDumpThread()
{
	m_logicTimeoutThread.Start(true);
}

void CThreadServer::ProcessCommand()
{
	while (true)
	{
		::Sleep(1);
	}
}

void CThreadServer::OnCommand( const XString& strCmd, const std::vector<XString>& params )
{

}

void CThreadServer::ExitServer()
{
	m_logicThread.SetStop(true);
	//m_logicTimeoutThread.SetStop(true);

	m_logicThread.Wait();
	//m_logicTimeoutThread.Wait();
	TSingleton<CIOCPSessionWorkThread>::GetInstance().EndWorkThread();
	TSingleton<CIOCPSessionIdleRunThread>::GetInstance().EndIdleRunThread();
}

void CThreadServer::Dump()
{

}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)