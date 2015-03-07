#ifndef APPLICATION_SYSTEMMONITORLIB_COMPUTERINFO_H__INCLUDED
#define APPLICATION_SYSTEMMONITORLIB_COMPUTERINFO_H__INCLUDED

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include "Hardware/CPUInfo.h"
#include "Hardware/GraphicsCardInfo.h"
#include "Hardware/HardDiskInfo.h"
#include "Hardware/MotherBoardInfo.h"
#include "Hardware/NetworkCardInfo.h"
#include "Hardware/MemoryInfo.h"
#include "Hardware/SoundCardInfo.h"
#include "OS/OperatingSystemInfo.h"

namespace Application
{
namespace SystemMonitor
{
    using Application::SystemMonitor::CPUInfo;
    using Application::SystemMonitor::GraphicsCardInfo;
    using Application::SystemMonitor::HardDiskInfo;
    using Application::SystemMonitor::MotherBoardInfo;
    using Application::SystemMonitor::NetworkCardInfo;
    using Application::SystemMonitor::MemoryInfo;
    using Application::SystemMonitor::SoundCardInfo;
    using Application::SystemMonitor::ProcessInfo;
    using Application::SystemMonitor::OperatingSystemInfo;

    class ComputerInfo
    {
    public:
        ComputerInfo();
        ~ComputerInfo();

        void Update();

        CPUInfo                             GetCPUInfo() const;
        HardDiskInfo                        GetHardDiskInfo() const;
        MotherBoardInfo                     GetMotherBoardInfo() const;
        GraphicsCardInfo                    GetGraphicsCardInfo() const;
        std::vector<MemoryInfo>             GetMemoryInfos() const;
        std::vector<SoundCardInfo>          GetSoundCardInfos() const;
        std::vector<NetworkCardInfo>        GetNetworkCardInfos() const;
        OperatingSystemInfo                 GetOperatingSystemInfo() const;

    private:
        CPUInfo                             mCPUInfo;
        HardDiskInfo                        mHardDiskInfo;
        MotherBoardInfo                     mMotherBoardInfo;
        GraphicsCardInfo                    mGraphicsCardInfo;
        std::vector<MemoryInfo>             mMemoryInfos;
        std::vector<SoundCardInfo>          mSoundCardInfos;
        std::vector<NetworkCardInfo>        mNetworkCardInfos;
        OperatingSystemInfo                 mOperatingSystemInfo;

        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int /* file_version */)
        {
            ar & mCPUInfo;
            ar & mHardDiskInfo;
            ar & mMotherBoardInfo;
            ar & mGraphicsCardInfo;
            ar & mMemoryInfos;
            ar & mSoundCardInfos;
            ar & mNetworkCardInfos;
            ar & mOperatingSystemInfo;
        }
    };
}
}

#endif