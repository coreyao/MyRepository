#ifndef APPLICATION_SYSTEMMONITORLIB_OPERATINGSYSTEMINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_OPERATINGSYSTEMINFO_H__INCLUDED

#include "OS/ProcessInfo.h"

namespace Application
{
namespace SystemMonitor
{
    class OperatingSystemInfo
    {
    public:
        OperatingSystemInfo();
        ~OperatingSystemInfo();

        void Update();

        XString GetOSName() const;
        std::vector<ProcessInfo> GetProcessInfos() const;
        size_t GetCurrentTotalCPUUsage() const;
        size_t GetCurrnetTotalMemoryUsage() const;

    private:
        XString mOSName;
        std::vector<ProcessInfo> mProcessInfos;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int /* file_version */)
        {
            ar & mOSName;
            ar & mProcessInfos;
        }
    };
}
}

#endif