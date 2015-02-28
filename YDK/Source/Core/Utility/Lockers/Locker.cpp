#include <cassert>

#include "Lockers/Locker.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CLocker::CLocker()
: mMutex(NULL)
{
#ifdef WIN32
    mMutex = CreateMutex(NULL, false, NULL);
    assert(mMutex != NULL);
#else
    //linux
#endif
}

CLocker::~CLocker()
{
#ifdef WIN32
    BOOL ret = CloseHandle(mMutex);
    assert(ret);
#else
    //linux
#endif
}

void CLocker::Lock()
{
#ifdef WIN32
    DWORD ret = WaitForSingleObject(mMutex, INFINITE);
    assert(ret != 0xffffffff);
#else
    //linux
#endif
}

void CLocker::Unlock()
{
#ifdef WIN32
    BOOL ret = ReleaseMutex(mMutex);
    assert(ret);
#else
    //linux
#endif
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
