#ifndef CORE_UTILITY_GUARD_H__INCLUDED
#define CORE_UTILITY_GUARD_H__INCLUDED

#include "BasicDefines.h"
#include "UtilityTypes.h"

namespace Core{ namespace Utility { class CLockerBase; } }

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CGuard
{
public:
    explicit CGuard(CLockerBase* lock);
    ~CGuard();

private:
    CLockerBase* mLocker;

    CGuard();
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif