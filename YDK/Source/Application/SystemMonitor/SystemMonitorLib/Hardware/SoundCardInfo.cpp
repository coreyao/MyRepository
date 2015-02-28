#include "Precomp/Precomp.h"
#include "Hardware/SoundCardInfo.h"
#include "SystemInfoObserver/SystemInfoObserver.h"

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

std::vector<SoundCardInfo> SoundCardInfo::sSoundCardInfoArray;

std::vector<SoundCardInfo> SoundCardInfo::GetSoundCardInfos()
{
    return sSoundCardInfoArray;
}

SoundCardInfo::~SoundCardInfo()
{
}

SoundCardInfo::SoundCardInfo()
{
}

void SoundCardInfo::Update()
{
    STRINGS temp;
    temp.push_back(TEXT("Manufacturer"));
    temp.push_back(TEXT("Name"));
    STRINGS_ARRAY valuesArray = SystemInfoObserver::GetInstance()->GetStringValueArrayWithClassAndAttibuteArray(TEXT("Win32_SoundDevice"), temp);
    if (valuesArray.size() >= 2)
    {
        STRINGS valuesManufacturer = valuesArray[0];
        STRINGS variantsProductName = valuesArray[1];

        std::vector<SoundCardInfo> tempSoundCardInfoArray;
        for (size_t i = 0; i < valuesManufacturer.size(); ++i)
        {
            SoundCardInfo currentSoundCard;
            currentSoundCard.mCompanyName = valuesManufacturer[i];
            currentSoundCard.mProductName = variantsProductName[i];

            tempSoundCardInfoArray.push_back(currentSoundCard);
        }

        sSoundCardInfoArray = tempSoundCardInfoArray;
    }
}

END_NAMESPACE_APPLICATION_SYSTEMMONITOR