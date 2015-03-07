#include "Precomp/Precomp.h"
#include "OS/OperatingSystemInfo.h"
#include "SystemInfoObserver/SystemInfoObserver.h"

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

OperatingSystemInfo::OperatingSystemInfo()
{
}

OperatingSystemInfo::~OperatingSystemInfo()
{
}

void OperatingSystemInfo::Update()
{
    STRINGS values = SystemInfoObserver::GetInstance()->GetStringValueWithClassAndAttribute(TEXT("Win32_OperatingSystem"), TEXT("Name"));
    mOSName = values[0];

    mProcessInfos = ProcessInfo::GetProcessInfos();
}

XString OperatingSystemInfo::GetOSName() const
{
    return mOSName;
}

std::vector<ProcessInfo> OperatingSystemInfo::GetProcessInfos() const
{
    return mProcessInfos;
}

size_t OperatingSystemInfo::GetCurrentTotalCPUUsage() const
{
    size_t totalCPUUsage = 0;
    std::vector<ProcessInfo>::const_iterator iter = mProcessInfos.begin();
    std::vector<ProcessInfo>::const_iterator endIter = mProcessInfos.end();
    for (; iter != endIter; ++iter)
    {
        if (iter->GeProcessName() == TEXT("Idle")) continue;

        totalCPUUsage += iter->GetCurrentCPUUsage();
    }

    return totalCPUUsage;
}

size_t OperatingSystemInfo::GetCurrnetTotalMemoryUsage() const
{
    size_t totalMemoryUsage = 0;
    std::vector<ProcessInfo>::const_iterator iter = mProcessInfos.begin();
    std::vector<ProcessInfo>::const_iterator endIter = mProcessInfos.end();
    for (; iter != endIter; ++iter)
    {
        totalMemoryUsage += iter->GetCurrentMemoryUsage();
    }

    return totalMemoryUsage;
}

END_NAMESPACE_APPLICATION_SYSTEMMONITOR