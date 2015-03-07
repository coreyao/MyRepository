#ifndef CORE_UTILITY_XTHREAD_H__INCLUDED
#define CORE_UTILITY_XTHREAD_H__INCLUDED

#include "BasicDefines.h"
#include "UtilityTypes.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class XThread
{
public:
	XThread();
	virtual ~XThread();

	// Start executing the thread
	void Start(bool bLoop = false);

	// Blocks the thread until the thread finished execution or time elapsed
	bool Wait(unsigned long time = -1);

	// [static] Causes the current thread to sleep for msecs milliseconds
	static void MSleep(unsigned int msecs);

	bool IsLoop() const { return mIsLoop; }
	bool IsStop() const { return mIsStop; }
	void SetStop(bool bStop) { mIsStop = bStop; }

protected:
	// The starting point for the thread.After calling start(),the newly created thread calls this function.
	// You can reimplemented this function to do other useful work. 
	// Returning from this method will end the execution of the thread.
	virtual void Run();

private:
#ifdef WIN32
	static unsigned __stdcall ThreadProc(void*);
	uint32 mThread;
#else
	//linux
#endif
	bool mIsLoop;
	bool mIsStop;
	
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif