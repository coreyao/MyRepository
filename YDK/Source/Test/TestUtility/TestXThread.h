#include "XThread/XThread.h"

using Core::Utility::XThread;

class TestThread : public XThread
{
public:
	virtual void Run();

private:
	int mThreadArg;
};

void TestThread::Run()
{
	for (int i = 0; i < 10000000; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			mThreadArg = 10;
			MSleep(1000);
		}
	}
}