#include "Precomp/Precomp.h"
#include "Hardware/GraphicsCardInfo.h"
#include "SystemInfoObserver/SystemInfoObserver.h"

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

GraphicsCardInfo::GraphicsCardInfo()
{
}

GraphicsCardInfo::~GraphicsCardInfo()
{
}

void GraphicsCardInfo::Update()
{
    STRINGS queryAttributes;
    queryAttributes.push_back(TEXT("Name"));
    queryAttributes.push_back(TEXT("Name"));

    STRINGS_ARRAY valuesArray = SystemInfoObserver::GetInstance()->GetStringValueArrayWithClassAndAttibuteArray(TEXT("Win32_VideoController"), queryAttributes);
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