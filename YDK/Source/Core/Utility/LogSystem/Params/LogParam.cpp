#define  WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Params/LogParam.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CLogParam::CLogParam()
: mLogTime(0)
, mMessage(TEXT(""))
{
}

CLogParam::~CLogParam()
{
}

CLogParam::CLogParam(const CLogParam& other)
{
    if (this != &other)
    {
        *this = other;
    }
}

CLogParam& CLogParam::operator =(const CLogParam& other)
{
    if (this != &other)
    {
        this->mLogTime = other.mLogTime;
        this->mMessage = other.mMessage;
    }

    return *this;
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
