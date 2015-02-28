#ifndef CORE_UTILITY_NETWORK_IOCP_IOCPSESSIONWORKTHREAD_H__INCLUDED
#define CORE_UTILITY_NETWORK_IOCP_IOCPSESSIONWORKTHREAD_H__INCLUDED

#include <list>

#include "XThread/XThread.h"
#include "IOCompletionPort.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CIOCPSessionWorkThread;
class CIOCPWorkThread : public XThread
{
public:
	CIOCPWorkThread();
	~CIOCPWorkThread();

	virtual void Run();
	void SetWorkThread(CIOCPSessionWorkThread* val) { m_workThread = val; }

private:
	CIOCPSessionWorkThread* m_workThread;
};

class CIOCPSessionWorkThread
{
public:
	CIOCPSessionWorkThread();
	virtual ~CIOCPSessionWorkThread();

	CIOCompletionPort& GetIOCP();
	uint32 GetWorkThreadNum() const;
	uint32 GetIOCPThreadNum() const;
	void EndWorkThread();

protected:
	bool StartWorkThread();

	unsigned int m_unWorkThreadCount;
	CIOCompletionPort m_IOCP;
	std::list<CIOCPWorkThread*> m_workThreads;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif