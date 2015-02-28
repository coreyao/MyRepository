#include "Precomp/Precomp.h"
#include "Hardware/HardDiskInfo.h"
#include "SystemInfoObserver/SystemInfoObserver.h"

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

namespace 
{
    const int KB = 1024;
    const int MB = 1024 * KB;
    const int GB = 1024 * MB;
}

HardDiskInfo::HardDiskInfo()
: mPartitionCount(0)
, mTotalFreespace(0)
, mTotalSize(0)
{
}

HardDiskInfo::~HardDiskInfo()
{
}

void HardDiskInfo::Update()
{
    STRINGS queryAttributes;
    queryAttributes.push_back(TEXT("Caption"));
    queryAttributes.push_back(TEXT("Caption"));
    queryAttributes.push_back(TEXT("Size"));
    STRINGS_ARRAY valuesArrayBasic = SystemInfoObserver::GetInstance()->GetStringValueArrayWithClassAndAttibuteArray(TEXT("Win32_DiskDrive"), queryAttributes);
    if (valuesArrayBasic.size() >= 3)
    {
        if (valuesArrayBasic[0].size() >= 1)
        {
            mCompanyName = valuesArrayBasic[0][0];
        }

        if (valuesArrayBasic[1].size() >= 1)
        {
            mProductName = valuesArrayBasic[1][0];
        }

        if (valuesArrayBasic[2].size() >= 1)
        {
            __int64 tempTotalSizeInBytes = valuesArrayBasic[2][0].ConvertToUInt64();
            mTotalSize = static_cast<float>(tempTotalSizeInBytes)/GB;
        }
    }

    //Set PartitionInfos
    std::vector<XString> temp;
    temp.push_back(TEXT("Name"));
    temp.push_back(TEXT("VolumeName"));
    temp.push_back(TEXT("Size"));
    temp.push_back(TEXT("FreeSpace"));
    STRINGS_ARRAY valuesArray = SystemInfoObserver::GetInstance()->GetStringValueArrayWithClassAndAttibuteArray(TEXT("Win32_LogicalDisk"), temp);
    if (valuesArray.size() >= 4)
    {
        STRINGS valuesName = valuesArray[0];
        STRINGS valuesVolumeName = valuesArray[1];
        STRINGS valuesSize = valuesArray[2];
        STRINGS valuesFreeSpace = valuesArray[3];

        __int64 tempTotalSizeInBytes = 0;
        XString totalSizeInBytes;
        for (size_t i = 0; i < valuesName.size(); ++i)
        {
            PartitionInfo currentPartition;
            currentPartition.mPartitionName = valuesName[i];
            currentPartition.mPartitionVolumeName = valuesVolumeName[i];

            totalSizeInBytes = valuesSize[i];
            tempTotalSizeInBytes = totalSizeInBytes.ConvertToUInt64();
            currentPartition.mPartitionSize = static_cast<float>(tempTotalSizeInBytes)/GB;
            if (currentPartition.mPartitionSize == 0.0f) 
                continue;
            totalSizeInBytes = valuesFreeSpace[i];
            tempTotalSizeInBytes = totalSizeInBytes.ConvertToUInt64();
            currentPartition.mPartitionFreespace = static_cast<float>(tempTotalSizeInBytes)/GB;

            mPartitionInfos.push_back(currentPartition);
            mTotalFreespace += currentPartition.mPartitionFreespace;

            ++mPartitionCount;
        }
    }
    

  
}
END_NAMESPACE_APPLICATION_SYSTEMMONITOR