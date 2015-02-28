#ifndef CORE_LOGSYSTEM_CONSOLEAPPENDER_H__INCLUDED
#define CORE_LOGSYSTEM_CONSOLEAPPENDER_H__INCLUDED

#include "Appenders/LogAppender.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CConsoleAppender : public CLogAppender
{
public:
    CConsoleAppender();
    virtual ~CConsoleAppender();

    virtual void Log(const CLogParam& logParam);
    virtual void Flush();

private:
    DISALLOW_COPY_AND_ASSIGN(CConsoleAppender);
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif