#include <Windows.h>
#include <iostream>
#include <fstream>
#include <ctime>

#include "XString/XString.h"
#include "TestXThread.h"

using Core::Utility::XString;
using Core::Utility::XChar;


void TestThreadFunction()
{
	TestThread testThread;
	testThread.Start();

	for (int i = 0; i < 10000; ++i)
	{
		std::cout << "Hello World ";
		XThread::MSleep(1000);
	}
}

int main()
{
	TestThreadFunction();
	
    system("pause");
    return 0;
}