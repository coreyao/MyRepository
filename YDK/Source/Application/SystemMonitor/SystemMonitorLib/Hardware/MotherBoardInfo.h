#ifndef APPLICATION_SYSTEMMONITORLIB_MOTHERBOARDINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_MOTHERBOARDINFO_H__INCLUDED

#include "Hardware/HardwareInfo.h"

namespace Application
{
namespace SystemMonitor
{
    class MotherBoardInfo : public HardwareInfo
    {
    public:
        MotherBoardInfo();
        ~MotherBoardInfo();

        void Update();

    private:
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