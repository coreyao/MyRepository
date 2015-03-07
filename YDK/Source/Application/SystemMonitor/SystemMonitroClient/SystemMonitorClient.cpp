#include <WinSock2.h>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>

#include "LogSystem.h"
#include "ComputerInfo.h"
#include "XString/XString.h"

using Application::SystemMonitor::ComputerInfo;
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

using Core::Utility::XString;

const char* clientIP = "127.0.0.1";
const unsigned short clientPort = 4567;
const unsigned int OneSecond = 1000;
ComputerInfo gComputer;

#ifdef UNICODE
#define XCout std::wcout
#else
#define XCout std::cout
#endif

#define DLL_EXPORT extern "C" _declspec(dllexport)

DLL_EXPORT ComputerInfo GetComputerInfo()
{
    return gComputer;
}

void Init()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) 
    {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        XString message;
        message.Format(TEXT("WSAStartup failed with error: %d\n"), err);
        Core::LOG_ERROR(message.c_str());
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        XString message;
        message.Format(TEXT("Could not find a usable version of Winsock.dll\n"));
        Core::LOG_ERROR(message.c_str());
        WSACleanup();
    }
}

int Recv(SOCKET s, char* recvBuffer, size_t len)
{
    size_t leftBytes = len;
    size_t offset = 0;
    int ret = 0;

    while (leftBytes > 0)
    {
        int recivedBytes = recv(s, recvBuffer + offset, leftBytes, 0);
        if (SOCKET_ERROR == recivedBytes)
        {
            ret = SOCKET_ERROR;
            Core::LOG_ERROR(TEXT("Receive failed"));
            break;
        }
        else if (0 == recivedBytes)
        {
            Core::LOG(TEXT("Connection is closed"));
            break;
        }

        leftBytes -= recivedBytes;
        offset += recivedBytes;
        ret += recivedBytes;
    }

    return ret;
}

//DLL_EXPORT int ProcessComputerInfo()
int main()
{
    //Initialize Log system
    Core::INIT_LOGSYSTEM;

    //Initialize Winsock2
    Init();

    //Establish connection
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == listenSock)
    {
        XString message;
        message.Format(TEXT("Create receiverSock failed %d"), WSAGetLastError());
        Core::LOG_ERROR(message.c_str());
        return EXIT_FAILURE;
    }

    sockaddr_in sockAddr;
    sockAddr.sin_addr.S_un.S_addr = inet_addr(clientIP);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(clientPort);

    int ret = bind(listenSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    if (SOCKET_ERROR == ret)
    {
        XString message;
        message.Format(TEXT("Bind failed %d"), WSAGetLastError());
        Core::LOG_ERROR(message.c_str());
        return EXIT_FAILURE;
    }

    ret = listen(listenSock, 5);
    if (SOCKET_ERROR == ret)
    {
        XString message;
        message.Format(TEXT("Listen failed %d"), WSAGetLastError());
        Core::LOG_ERROR(message.c_str());
        return EXIT_FAILURE;
    }

    Core::LOG(TEXT("Listening..."));

    sockaddr_in serverAddr;
    int len = sizeof(sockaddr_in);
    SOCKET sockServer = accept(listenSock, (SOCKADDR*)&serverAddr, &len);
    if (INVALID_SOCKET == sockServer)
    {
        XString message;
        message.Format(TEXT("Accept failed %d"), WSAGetLastError());
        Core::LOG_ERROR(message.c_str());
        return EXIT_FAILURE;
    }

    Core::LOG(TEXT("Connection established"));


    while (true)
    {
        Sleep(2000);

        //Receive Computer information
        size_t recvBufferLen = 0;
        ret = Recv(sockServer, (char*)&recvBufferLen, sizeof(size_t));
        if (INVALID_SOCKET == ret)
        {
            return EXIT_FAILURE;
        }

        char* recvBuffer = new char[recvBufferLen]();
        ret = Recv(sockServer, recvBuffer, recvBufferLen);
        if (INVALID_SOCKET == ret)
        {
            return EXIT_FAILURE;
        }
        else if (0 == ret)
        {
            break;
        }

        std::istringstream iss(std::string(recvBuffer, recvBufferLen));
        boost::archive::text_iarchive ia(iss);
        ia >> gComputer;

    }

    closesocket(sockServer);

    return EXIT_SUCCESS;
}

// struct MockedPartitionInfo
// {
//     XString mPartitionName;
//     XString mPartitionVolumeName;
//     float mPartitionSize;
//     float mPartitionFreespace;
// };
// 
// struct MockedHardDiskInfo
// {
//     float   mTotalSize;
//     float   mTotalFreespace;
//     size_t  mPartitionCount;
//     MockedPartitionInfo* mPartitionInfos;
// };
// 
// struct MockedOperatingSystemInfo
// {
//     XString         mOSName;
//     size_t          mProcessInfoCount;
//     ProcessInfo*    mProcessInfos;
// };
// 
// struct MockedComputerInfo
// {
//     CPUInfo                             mCPUInfo;
//     MockedHardDiskInfo                  mHardDiskInfo;
//     MotherBoardInfo                     mMotherBoardInfo;
//     GraphicsCardInfo                    mGraphicsCardInfo;
// 
//     size_t                              mMemoryInfoCount;
//     MemoryInfo*                         mMemoryInfos;
// 
//     size_t                              mSoundCardInfoCount;
//     SoundCardInfo*                      mSoundCardInfos;
// 
//     size_t                              mNetworkCardInfoCount;
//     NetworkCardInfo*                    mNetworkCardInfos;
// 
//     MockedOperatingSystemInfo           mOperatingSystemInfo;
// };
// 
// MockedComputerInfo MockComputerInfo(const ComputerInfo& computer)
// {
//     
// };
