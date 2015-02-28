#ifndef CORE_UTILITY_NETWORK_IOCP_IOCPSESSIONRUNTHREAD_H__INCLUDED
#define CORE_UTILITY_NETWORK_IOCP_IOCPSESSIONRUNTHREAD_H__INCLUDED

#include <list>

#include "IOCompletionPort.h"
#include "XThread/XThread.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CIOCPSessionIdleRunThread;
class CIOCPIdleRunThread : public XThread
{
public:
	CIOCPIdleRunThread();
	~CIOCPIdleRunThread();

	virtual void Run();
	void SetIdleRunThread(CIOCPSessionIdleRunThread* val) { m_idleRunThread = val; }

private:
	CIOCPSessionIdleRunThread* m_idleRunThread;
};

class CIOCPSessionIdleRunThread
{
public:
	CIOCPSessionIdleRunThread();
	virtual ~CIOCPSessionIdleRunThread();

	CIOCompletionPort& GetIOCP();
	uint32 GetIdleRunThreadNum() const;
	uint32 GetIOCPThreadNum() const;

	void EndIdleRunThread();

protected:
	bool StartIdleRunThread();

	unsigned int m_unIdleRunThreadCount;
	CIOCompletionPort m_IOCP;
	std::list<CIOCPIdleRunThread*> m_workThreads;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif