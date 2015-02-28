#include "Precomp/Precomp.h"
#include "Hardware/MemoryInfo.h"
#include "SystemInfoObserver/SystemInfoObserver.h"

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

namespace
{
    const int KB = 1024;
    const int MB = 1024 * KB;
    const int GB = 1024 * MB;
}

std::vector<MemoryInfo> MemoryInfo::sMemoryInfoArray;

std::vector<MemoryInfo> MemoryInfo::GetMemroyInfos()
{
    return sMemoryInfoArray;
}

MemoryInfo::MemoryInfo()
{
}

MemoryInfo::~MemoryInfo()
{

}

size_t MemoryInfo::GetTotalSize() const
{
    return mTotalSize;
}

void MemoryInfo::Update()
{
    STRINGS temp;
    temp.push_back(TEXT("Manufacturer"));
    temp.push_back(TEXT("Name"));
    temp.push_back(TEXT("Capacity"));
    STRINGS_ARRAY valuesArray = SystemInfoObserver::GetInstance()->GetStringValueArrayWithClassAndAttibuteArray(TEXT("Win32_PhysicalMemory"), temp);
    if (valuesArray.size() >= 3)
    {
        STRINGS valuesManufacturer = valuesArray[0];
        STRINGS valuesName = valuesArray[1];
        STRINGS valuesCapacity = valuesArray[2];

        std::vector<MemoryInfo> tempMemoryInfoArray;
        for (size_t i = 0; i < valuesManufacturer.size(); ++i)
        {
            MemoryInfo currentMemory;
            currentMemory.mCompanyName = valuesManufacturer[i];
            currentMemory.mProductName = valuesName[i];
            XString totalSizeInBytes = valuesCapacity[i];
            __int64 tempTotalSizeInBytes = totalSizeInBytes.ConvertToUInt64();
            currentMemory.mTotalSize = static_cast<size_t>(tempTotalSizeInBytes/MB);

            tempMemoryInfoArray.push_back(currentMemory);
        }
        sMemoryInfoArray = tempMemoryInfoArray;
    }
}

END_NAMESPACE_APPLICATION_SYSTEMMONITOR
