#include "Lockers/Guard.h"
#include "Lockers/Locker.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CGuard::CGuard(CLockerBase* locker)
: mLocker(locker)
{
    mLocker->Lock();
}

CGuard::~CGuard()
{
    mLocker->Unlock();
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)