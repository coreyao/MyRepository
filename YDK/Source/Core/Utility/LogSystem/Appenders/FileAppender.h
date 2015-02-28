#ifndef CORE_LOGSYSTEM_FILEAPPENDER_H__INCLUDED
#define CORE_LOGSYSTEM_FILEAPPENDER_H__INCLUDED

#include "Appenders/LogAppender.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CFileAppender : public CLogAppender
{
public:
    CFileAppender();
    virtual ~CFileAppender();

    virtual void Log(const CLogParam& logParam);
    virtual void Flush();

private:
    DISALLOW_COPY_AND_ASSIGN(CFileAppender);
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif