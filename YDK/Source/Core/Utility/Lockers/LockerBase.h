#ifndef CORE_UTILITY_LOCKERBASE_H__INCLUDED
#define CORE_UTILITY_LOCKERBASE_H__INCLUDED

#include "BasicDefines.h"
#include "UtilityTypes.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CLockerBase
{
public:
    CLockerBase();
    virtual ~CLockerBase();

    virtual void Lock() = 0;
    virtual void Unlock() = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(CLockerBase);
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif