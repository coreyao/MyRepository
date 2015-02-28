#ifndef APPLICATION_SYSTEMMONITORLIB_GRAPHICCARDINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_GRAPHICCARDINFO_H__INCLUDED

#include "Hardware/HardwareInfo.h"

namespace Application
{
namespace SystemMonitor
{
    class GraphicsCardInfo : public HardwareInfo
    {
    public:
        GraphicsCardInfo();
        ~GraphicsCardInfo();

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