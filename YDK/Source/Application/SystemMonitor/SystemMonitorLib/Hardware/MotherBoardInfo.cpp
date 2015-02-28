#include "Precomp/Precomp.h"
#include "Hardware/MotherBoardInfo.h"
#include "SystemInfoObserver/SystemInfoObserver.h"

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

MotherBoardInfo::MotherBoardInfo()
{
}

MotherBoardInfo::~MotherBoardInfo()
{
}

void MotherBoardInfo::Update()
{
    STRINGS queryAttributes;
    queryAttributes.push_back(TEXT("Manufacturer"));
    queryAttributes.push_back( TEXT("Product"));

    STRINGS_ARRAY valuesArray = SystemInfoObserver::GetInstance()->GetStringValueArrayWithClassAndAttibuteArray(TEXT("Win32_BaseBoard"), queryAttributes);
    if (valuesArray.size() >= 2)
    {
        if (valuesArray[0].size() >= 1)
        {
            mCompanyName = valuesArray[0][0];
        }
        if (valuesArray[1].size() >= 1)
        {
            mProductName = valuesArray[1][0];
        }
    }
}
END_NAMESPACE_APPLICATION_SYSTEMMONITOR
