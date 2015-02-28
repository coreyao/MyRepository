#ifndef CORE_UTILITY_LOCKER_H__INCLUDED
#define CORE_UTILITY_LOCKER_H__INCLUDED

#define  WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Lockers/LockerBase.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CLocker : public CLockerBase
{
public:
    CLocker();
    virtual ~CLocker();

    virtual void Lock();
    virtual void Unlock();

private:
#ifdef WIN32
    HANDLE mMutex;
#else
    //linux
#endif

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif