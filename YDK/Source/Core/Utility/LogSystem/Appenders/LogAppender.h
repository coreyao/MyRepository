#ifndef CORE_LOGSYSTEM_LOGAPPENDER_H__INCLUDED
#define CORE_LOGSYSTEM_LOGAPPENDER_H__INCLUDED

#include "Params/LogParam.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CLogAppender
{
public:
    CLogAppender();
    virtual ~CLogAppender();

    virtual void Log(const CLogParam& logParam) = 0;
    virtual void Flush() = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(CLogAppender);
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif