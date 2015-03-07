#ifndef CORE_LOGSYSTEM_LOGPARAM_H__INCLUDED
#define CORE_LOGSYSTEM_LOGPARAM_H__INCLUDED

#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

using Core::Utility::XString;
// LogParam can be derived
class CLogParam
{
public:
    CLogParam();
    virtual ~CLogParam();

    CLogParam(const CLogParam& other);
    CLogParam& operator=(const CLogParam& other);

    time_t GetLogTime() const { return mLogTime; }
    void SetLogTime(time_t val) { mLogTime = val; }

    XString GetMessage() const { return mMessage; }
    void SetMessage(const XString& val) { mMessage = val; }

private:
    time_t  mLogTime;
    XString mMessage;
   
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif