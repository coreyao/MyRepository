#ifndef APPLICATION_SYSTEMMONITORLIB_NETWORKCARDINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_NETWORKCARDINFO_H__INCLUDED

#include "Hardware/HardwareInfo.h"
#include <vector>

namespace Application
{
namespace SystemMonitor
{
    class NetworkCardInfo : public HardwareInfo
    {
    public:
        static std::vector<NetworkCardInfo> GetNetworkCards();
        static void Update();

        XString GetMacAddress() const { return mMacAddress; }

        NetworkCardInfo();
        ~NetworkCardInfo();

    private:
        XString mMacAddress;
        static std::vector<NetworkCardInfo> sNetworkCardInfoArray;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int /* file_version */)
        {
            ar & boost::serialization::base_object<HardwareInfo>(*this);
            ar & mMacAddress;
        }
    };
}
}

#endif