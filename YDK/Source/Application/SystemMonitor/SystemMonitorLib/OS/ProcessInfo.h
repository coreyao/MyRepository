#ifndef APPLICATION_SYSTEMMONITORLIB_PROCESSINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_PROCESSINFO_H__INCLUDED

#include <vector>
#include "XString/XString.h"

using Core::Utility::XString;
using Core::XChar;

namespace Application
{
namespace SystemMonitor
{
    class ProcessInfo
    {
    public:
        static std::vector<ProcessInfo> GetProcessInfos();
        XString GeProcessName() const;
        size_t GetCurrentCPUUsage() const;
        size_t GetCurrentMemoryUsage() const;
        size_t GetPID() const;

        ~ProcessInfo();

    private:
        ProcessInfo();

        XString mProcessName;
        size_t mCurrentCPUUsage;
        size_t mCurrentMemoryUsage;
        size_t mPID;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int /* file_version */)
        {
            ar & mProcessName & mCurrentCPUUsage & mCurrentMemoryUsage & mPID;
        }
    };
}
}

#endif