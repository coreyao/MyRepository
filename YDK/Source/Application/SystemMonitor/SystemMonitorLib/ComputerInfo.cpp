#include "Precomp/Precomp.h"
#include "ComputerInfo.h"

BEGIN_NAMESPACE_APPLICATION_SYSTEMMONITOR

CPUInfo ComputerInfo::GetCPUInfo() const
{
    return mCPUInfo;
}

HardDiskInfo ComputerInfo::GetHardDiskInfo() const
{
    return mHardDiskInfo;
}

MotherBoardInfo ComputerInfo::GetMotherBoardInfo() const
{
    return mMotherBoardInfo;
}

GraphicsCardInfo ComputerInfo::GetGraphicsCardInfo() const
{
    return mGraphicsCardInfo;
}

std::vector<MemoryInfo> ComputerInfo::GetMemoryInfos() const
{
    return mMemoryInfos;
}

std::vector<SoundCardInfo> ComputerInfo::GetSoundCardInfos() const
{
    return mSoundCardInfos;
}

std::vector<NetworkCardInfo> ComputerInfo::GetNetworkCardInfos() const
{
    return mNetworkCardInfos;
}

OperatingSystemInfo ComputerInfo::GetOperatingSystemInfo() const
{
    return mOperatingSystemInfo;
}

ComputerInfo::ComputerInfo()
{
}

ComputerInfo::~ComputerInfo()
{
}

void ComputerInfo::Update()
{
    mCPUInfo.Update();
    mHardDiskInfo.Update();
    mMotherBoardInfo.Update();
    mGraphicsCardInfo.Update();

    MemoryInfo::Update();
    mMemoryInfos = MemoryInfo::GetMemroyInfos();

    SoundCardInfo::Update();
    mSoundCardInfos = SoundCardInfo::GetSoundCardInfos();

    NetworkCardInfo::Update();
    mNetworkCardInfos = NetworkCardInfo::GetNetworkCards();

    mOperatingSystemInfo.Update();
}
END_NAMESPACE_APPLICATION_SYSTEMMONITOR