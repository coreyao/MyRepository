#include "Precomp/Precomp.h"
#include "Hardware/NetworkCardInfo.h"
#include "SystemInfoObserver/SystemInfoObserver.h"

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

std::vector<NetworkCardInfo> NetworkCardInfo::sNetworkCardInfoArray;

std::vector<NetworkCardInfo> NetworkCardInfo::GetNetworkCards()
{
    return sNetworkCardInfoArray;
}

NetworkCardInfo::NetworkCardInfo()
{
}

NetworkCardInfo::~NetworkCardInfo()
{
}

void NetworkCardInfo::Update()
{
    STRINGS temp;
    temp.push_back(TEXT("Manufacturer"));
    temp.push_back(TEXT("ProductName"));
    temp.push_back(TEXT("MAcaddress"));
    STRINGS_ARRAY valuesArray = SystemInfoObserver::GetInstance()->GetStringValueArrayWithClassAndAttibuteArray(TEXT("Win32_NetworkAdapter"), temp);
    if (valuesArray.size() >= 3)
    {
        STRINGS valuesManufacturer = valuesArray[0];
        STRINGS variantsProductName = valuesArray[1];
        STRINGS valuesMAcaddress = valuesArray[2];

        std::vector<NetworkCardInfo> tempNetworkCardInfoArray;
        for (size_t i = 0; i < valuesMAcaddress.size(); ++i)
        {
            NetworkCardInfo currentNetworkCard;
            if (valuesManufacturer[i] == TEXT("Microsoft") || valuesManufacturer[i] == TEXT("")) continue;
            currentNetworkCard.mCompanyName = valuesManufacturer[i];
            currentNetworkCard.mProductName = variantsProductName[i];
            currentNetworkCard.mMacAddress = valuesMAcaddress[i];

            tempNetworkCardInfoArray.push_back(currentNetworkCard);
        }

        sNetworkCardInfoArray = tempNetworkCardInfoArray;
    }
}


END_NAMESPACE_APPLICATION_SYSTEMMONITOR
