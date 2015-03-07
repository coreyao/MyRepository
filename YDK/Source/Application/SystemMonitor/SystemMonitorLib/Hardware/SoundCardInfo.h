#ifndef APPLICATION_SYSTEMMONITORLIB_SOUNDCARDINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_SOUNDCARDINFO_H__INCLUDED

#include "Hardware/HardwareInfo.h"

namespace Application
{
namespace SystemMonitor
{
    class SoundCardInfo : public HardwareInfo
    {
    public:
        static std::vector<SoundCardInfo> GetSoundCardInfos();
        static void Update();

        SoundCardInfo();
        ~SoundCardInfo();

    private:
        static std::vector<SoundCardInfo> sSoundCardInfoArray;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int /* file_version */)
        {
            ar & boost::serialization::base_object<HardwareInfo>(*this);
        }
    };
}
}

#endif