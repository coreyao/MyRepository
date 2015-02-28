#include "Precomp/Precomp.h"
#include "OS/ProcessInfo.h"
#include "SystemInfoObserver/SystemInfoObserver.h"

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

namespace
{
    const int KB = 1024;
    const int MB = 1024 * KB;
    const int GB = 1024 * MB;
}

ProcessInfo::ProcessInfo()
{
}

ProcessInfo::~ProcessInfo()
{
}

std::vector<ProcessInfo> ProcessInfo::GetProcessInfos()
{
    std::vector<ProcessInfo> result;
    std::vector<XString> temp;
    temp.push_back(TEXT("Name"));
    temp.push_back(TEXT("PercentProcessorTime"));
    temp.push_back(TEXT("WorkingSet"));
    STRINGS_ARRAY valuesArray = SystemInfoObserver::GetInstance()->GetStringValueArrayWithClassAndAttibuteArray(TEXT("Win32_PerfFormattedData_PerfProc_Process"), temp);
    STRINGS valuesName = valuesArray[0];
    STRINGS valuesCPU = valuesArray[1];
    STRINGS valuesMemory = valuesArray[2];

    for (size_t i = 0; i < valuesName.size(); ++i)
    {
        ProcessInfo currentProcessInfo;
        currentProcessInfo.mProcessName = valuesName[i];
        if (currentProcessInfo.mProcessName == TEXT("_Total")) continue;

        XString CPUUsage = valuesCPU[i];
        __int64 tempCPUUsage = CPUUsage.ConvertToUInt64();
        currentProcessInfo.mCurrentCPUUsage = static_cast<size_t>(tempCPUUsage);

        XString memoryUsage = valuesMemory[i];
        __int64 tempMemoryUsage = memoryUsage.ConvertToUInt64();
        currentProcessInfo.mCurrentMemoryUsage = static_cast<size_t>(tempMemoryUsage)/KB;

        result.push_back(currentProcessInfo);
    }

    return result;
}

XString ProcessInfo::GeProcessName() const
{
    return mProcessName;
}

size_t ProcessInfo::GetCurrentCPUUsage() const
{
    return mCurrentCPUUsage;
}

size_t ProcessInfo::GetCurrentMemoryUsage() const
{
    return mCurrentMemoryUsage;
}

size_t ProcessInfo::GetPID() const
{
    return mPID;
}

END_NAMESPACE_APPLICATION_SYSTEMMONITOR