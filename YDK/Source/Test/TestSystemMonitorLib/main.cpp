#include <windows.h>
#include <iostream>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "LogSystem.h"
#include "ComputerInfo.h"

using Application::SystemMonitor::CPUInfo;
using Application::SystemMonitor::GraphicsCardInfo;
using Application::SystemMonitor::HardDiskInfo;
using Application::SystemMonitor::MotherBoardInfo;
using Application::SystemMonitor::NetworkCardInfo;
using Application::SystemMonitor::MemoryInfo;
using Application::SystemMonitor::SoundCardInfo;
using Application::SystemMonitor::ProcessInfo;
using Application::SystemMonitor::OperatingSystemInfo;
using Application::SystemMonitor::ComputerInfo;

#ifdef UNICODE
#define XCout std::wcout
#else
#define XCout std::cout
#endif

int main()
{
    Core::INIT_LOGSYSTEM;

    ComputerInfo computerOriginal;
    computerOriginal.Update();

    std::ostringstream oss;
    boost::archive::text_oarchive oa(oss);
    oa << computerOriginal;

    XCout << oss.str().length();

    ComputerInfo computer;
    std::string temp = oss.str();
    const char* str = temp.c_str();
    std::istringstream iss(std::string(str, temp.length()));
    boost::archive::text_iarchive ia(iss);
    ia >> computer;

    XCout << TEXT("-----------------------------------CPU-----------------------------------------")<< std::endl;
    CPUInfo cpu = computer.GetCPUInfo();
    //XCout << TEXT("Current Usage: ") << cpu.GetCurrentUsage() << std::endl;
    XCout << TEXT("Company Name:  ") << cpu.GetCompanyName() << std::endl;
    XCout << TEXT("Product Name:  ") << cpu.GetProductName() << std::endl;

    XCout << TEXT("-----------------------------------Graphics-----------------------------------------")<< std::endl;
    GraphicsCardInfo video = computer.GetGraphicsCardInfo();
    XCout << TEXT("Company Name:  ") << video.GetCompanyName() << std::endl;
    XCout << TEXT("Product Name:  ") << video.GetProductName() << std::endl;

    XCout << TEXT("----------------------------------Harddisk---------------------------------------")<< std::endl;
    HardDiskInfo hardDisk = computer.GetHardDiskInfo();
    XCout << TEXT("Company Name:  ") << hardDisk.GetCompanyName() << std::endl;
    XCout << TEXT("Product Name:  ") << hardDisk.GetProductName() << std::endl;
    XCout << TEXT("Total   Size:  ") << hardDisk.GetTotalSize() << TEXT("(GB)") << std::endl;
    XCout << TEXT("Total   Free Space:  ") << hardDisk.GetTotalFreespace() << TEXT("(GB)") << std::endl;
    std::vector<HardDiskInfo::PartitionInfo> partitions = hardDisk.GetPartitionInfos();
    std::vector<HardDiskInfo::PartitionInfo>::const_iterator iter =partitions.begin();
    for (;iter != partitions.end(); ++iter)
    {
        XCout << TEXT("Partition Name:  ") << iter->mPartitionName << std::endl;
        XCout << TEXT("Partition VolumeName:  ") << iter->mPartitionVolumeName << std::endl;
        XCout << TEXT("Partition Size:  ") << iter->mPartitionSize << TEXT("(GB)") << std::endl;
        XCout << TEXT("Partition Freespace:  ") << iter->mPartitionFreespace << TEXT("(GB)") << std::endl;
    }

    XCout << TEXT("----------------------------------MotherBoard-------------------------------------")<< std::endl;
    MotherBoardInfo motherBoard = computer.GetMotherBoardInfo();
    XCout << TEXT("Company Name:  ") << motherBoard.GetCompanyName() << std::endl;
    XCout << TEXT("Product Name:  ") << motherBoard.GetProductName() << std::endl;

    XCout << TEXT("-----------------------------------Network---------------------------------------")<< std::endl;
    std::vector<NetworkCardInfo> networkCards = computer.GetNetworkCardInfos();
    std::vector<NetworkCardInfo>::const_iterator citer =networkCards.begin();
    for (; citer != networkCards.end(); ++citer)
    {
        XCout << TEXT("Company Name:  ") << citer->GetCompanyName() << std::endl;
        XCout << TEXT("Product Name:  ") << citer->GetProductName() << std::endl;
        XCout << TEXT("MAC Address:  ") << citer->GetMacAddress() << std::endl;
    }

    XCout << TEXT("-----------------------------------Memory-------------------------------------")<< std::endl;
    std::vector<MemoryInfo> memoryInfos = computer.GetMemoryInfos();
    std::vector<MemoryInfo>::const_iterator citer1 = memoryInfos.begin();
    for (; citer1 != memoryInfos.end(); ++citer1)
    {
        XCout << TEXT("Company Name:  ") << citer1->GetCompanyName() << std::endl;
        XCout << TEXT("Product Name:  ") << citer1->GetProductName() << std::endl;
        XCout << TEXT("Total size:  ") << citer1->GetTotalSize() << TEXT("(MB)") << std::endl;
    }
    //XCout << TEXT("Current Usage:  ") << MemoryInfo::GetCurrentUsage() << std::endl;

    XCout << TEXT("----------------------------------Sound---------------------------------------")<< std::endl;
    std::vector<SoundCardInfo> soundCardInfos = computer.GetSoundCardInfos();
    std::vector<SoundCardInfo>::const_iterator citer2 = soundCardInfos.begin();
    for (; citer2 != soundCardInfos.end(); ++citer2)
    {
        XCout << TEXT("Company Name:  ") << citer2->GetCompanyName() << std::endl;
        XCout << TEXT("Product Name:  ") << citer2->GetProductName() << std::endl;
    }

    XCout << TEXT("----------------------------------OS---------------------------------------")<< std::endl;
    OperatingSystemInfo os = computer.GetOperatingSystemInfo();
    XCout << os.GetOSName() << std::endl;

    XCout << TEXT("Total CPU Usage: ") << os.GetCurrentTotalCPUUsage() << TEXT("%") << std::endl
        << TEXT("Total Memory Usage: ") << os.GetCurrnetTotalMemoryUsage() << TEXT("KB") << std::endl;

    XCout << TEXT("----------------------------------Process---------------------------------------")<< std::endl;
    std::vector<ProcessInfo> processInfos = os.GetProcessInfos();
    std::vector<ProcessInfo>::const_iterator citer3 = processInfos.begin();
    for (; citer3 != processInfos.end(); ++citer3)
    {
      XCout << TEXT("Process Name: ") << citer3->GeProcessName() << TEXT("   ")
            << TEXT("CPU Usage: ") << citer3->GetCurrentCPUUsage() << TEXT("%  ")
            << TEXT("Memory Usage: ") << citer3->GetCurrentMemoryUsage() << TEXT("KB") <<std::endl;
    }

    system("pause");
    return 0;
}