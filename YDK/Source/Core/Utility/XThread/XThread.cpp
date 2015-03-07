#define  WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>

#include "XThread/XThread.h"
#include "LogSystem.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

XThread::XThread()
: mThread(0)
, mIsLoop(false)
, mIsStop(false)
{
#ifdef WIN32
	mThread = _beginthreadex(NULL, 0, ThreadProc, (void*)this, CREATE_SUSPENDED, NULL);
#else
	//linux
#endif
}

XThread::~XThread()
{
	mIsStop = true;
}

void XThread::Start(bool bLoop)
{
	mIsLoop = bLoop;
#ifdef WIN32
	ResumeThread((HANDLE)mThread);
#else
		//linux
#endif
}

void XThread::Run()
{
	//Overrided by derived class
}

unsigned XThread::ThreadProc( void* param)
{
	XThread* self = (XThread*)param;
	if (self->IsLoop())
	{
		while (!self->IsStop())
		{
			self->Run();
		}

		Core::LOG("Thread exit");
	}
	else
	{
		self->Run();
	}

	self->SetStop(true);

	return 0;
}

bool XThread::Wait( unsigned long time /*= -1*/ )
{
	bool result = true;
#ifdef WIN32
	DWORD ret = WaitForSingleObject((HANDLE)mThread, time);
	if (ret == WAIT_FAILED)
	{
		result = false;
	}
#else
	//linux
#endif

	return result;
}

void XThread::MSleep( unsigned int msecs )
{
#ifdef WIN32
	::Sleep(msecs);
#else
	//linux
#endif
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)