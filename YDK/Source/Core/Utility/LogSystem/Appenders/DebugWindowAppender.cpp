#define  WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Appenders/DebugWindowAppender.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CDebugWindowAppender::CDebugWindowAppender()
{
}

CDebugWindowAppender::~CDebugWindowAppender()
{
}

void CDebugWindowAppender::Log(const CLogParam& logParam)
{
#ifdef WIN32
    //OutputDebugString(logParam.GetLogTime());
    OutputDebugString(logParam.GetMessage().c_str());
    OutputDebugString(TEXT("\n"));
#endif
}

void CDebugWindowAppender::Flush()
{

}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)