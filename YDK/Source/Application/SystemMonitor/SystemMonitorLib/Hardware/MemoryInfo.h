#ifndef APPLICATION_SYSTEMMONITORLIB_MEMORYINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_MEMORYINFO_H__INCLUDED

#include "Hardware/HardwareInfo.h"
#include <vector>

namespace Application
{
namespace SystemMonitor
{
    class MemoryInfo : public HardwareInfo
    {
    public:
        static std::vector<MemoryInfo> GetMemroyInfos();
        static void Update();

        size_t GetTotalSize() const;

        MemoryInfo();
        ~MemoryInfo();

    private:
        size_t mTotalSize;
        static std::vector<MemoryInfo> sMemoryInfoArray;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int /* file_version */)
        {
            ar & boost::serialization::base_object<HardwareInfo>(*this);
            ar & mTotalSize;
        }
    };
}
}

#endif