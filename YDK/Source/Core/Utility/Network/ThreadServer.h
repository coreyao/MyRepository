#ifndef CORE_UTILITY_NETWORK_THREADSERVER_H__INCLUDED
#define CORE_UTILITY_NETWORK_THREADSERVER_H__INCLUDED

#include <Windows.h>
#include <DbgHelp.h>
#include <vector>
#include "XThread/XThread.h"
#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CThreadServer;
class CLogicThread : public XThread
{
public:
	CLogicThread();
	~CLogicThread();

	virtual void Run();
	void SetThreadServer(CThreadServer* val) { m_threadServer = val; }

private:
	CThreadServer* m_threadServer;
};

class LogicTimeoutThread : public XThread
{
public:
	LogicTimeoutThread();
	~LogicTimeoutThread();

	virtual void Run();
	void SetThreadServer(CThreadServer* val) { m_threadServer = val; }

private:
	CThreadServer* m_threadServer;
};

class CThreadServer
{
public:
	CThreadServer();
	virtual ~CThreadServer();

	virtual void Startup(bool bConsole = true, bool bOpenTimeoutDump = false, uint32 uTimeoutValue = 10000, MINIDUMP_TYPE eDumpType = MiniDumpNormal);
	virtual void Shutdown();
	virtual void Process() = 0;
	void Dump();

	uint64 GetLastProcLogicTime() const { return m_lastProcLogicTime; }
	void SetLastProcLogicTime(uint64 val) { m_lastProcLogicTime = val; }

	uint32 GetTimeoutValue() const { return m_uTimeoutValue; }
	void SetTimeoutValue(uint32 val) { m_uTimeoutValue = val; }

	bool GetOpenTimeoutDump() const { return m_bOpenTimeoutDump; }

protected:
	virtual void StartLogicThread();
	virtual void StartTimeoutDumpThread();
	virtual void ProcessCommand();
	virtual void OnCommand(const XString& strCmd, const std::vector<XString>& params);
	virtual void ExitServer();


	CLogicThread m_logicThread;
	LogicTimeoutThread m_logicTimeoutThread;

	bool m_bOpenTimeoutDump;
	uint64 m_lastProcLogicTime;
	uint32 m_uTimeoutValue;
	
	MINIDUMP_TYPE m_eDumpType;

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif

