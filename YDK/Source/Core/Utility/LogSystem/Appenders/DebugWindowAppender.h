#ifndef CORE_LOGSYSTEM_DEBUGWINDOWAPPENDER_H__INCLUDED
#define CORE_LOGSYSTEM_DEBUGWINDOWAPPENDER_H__INCLUDED

#include "Appenders/LogAppender.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CDebugWindowAppender : public CLogAppender
{
public:
    CDebugWindowAppender();
    virtual ~CDebugWindowAppender();

    virtual void Log(const CLogParam& logParam);
    virtual void Flush();

private:
    DISALLOW_COPY_AND_ASSIGN(CDebugWindowAppender);
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif