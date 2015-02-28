#include <iostream>

#include "Appenders/ConsoleAppender.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

namespace
{
#ifdef UNICODE
#define XCout std::wcout 
#else
#define XCout std::cout 
#endif
}

CConsoleAppender::CConsoleAppender()
{
}

CConsoleAppender::~CConsoleAppender()
{
}

void CConsoleAppender::Log(const CLogParam &logParam)
{

    XCout << logParam.GetMessage() << std::endl;
}

void CConsoleAppender::Flush()
{
    XCout.flush();
}


END_NAMESPACE(Utility)
END_NAMESPACE(Core)