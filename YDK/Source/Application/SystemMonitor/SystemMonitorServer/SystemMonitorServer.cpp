#include <WinSock2.h>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "LogSystem.h"
#include "ComputerInfo.h"
#include "XString/XString.h"

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

#ifdef UNICODE
#define XCout std::wcout
#else
#define XCout std::cout
#endif

int Send(SOCKET s, const char* sendBuffer, size_t len)
{
    size_t leftBytes = len;
    size_t offset = 0;
    int ret = len;

    while (leftBytes > 0)
    {
        int sentBytes = send(s, sendBuffer + offset, leftBytes, 0);
        if (SOCKET_ERROR == sentBytes)
        {
            ret = SOCKET_ERROR;
            Core::LOG_ERROR(TEXT("Send failed"));
            break;
        }
        leftBytes -= sentBytes;
        offset += sentBytes;
    }

    return ret;
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

int main()
{
    //Initialize Log system
    Core::INIT_LOGSYSTEM;

    //Initialize Winsock2
    Init();

    //Establish connection to server
    SOCKET senderSock = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == senderSock)
    {
        XString message;
        message.Format(TEXT("Create senderSock failed %d"), WSAGetLastError());
        Core::LOG_ERROR(message.c_str());
        return EXIT_FAILURE;
    }

    sockaddr_in sockAddr;
    sockAddr.sin_addr.S_un.S_addr = inet_addr(clientIP);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(clientPort);

    int ret = connect(senderSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    if (SOCKET_ERROR == ret)
    {
        Core::LOG_ERROR(TEXT("Connect failed"));
        return EXIT_FAILURE;
    }

    Core::LOG(TEXT("Connection established"));

    //Send the Operating System information to client
    while (true)
    {
        Sleep(2000);
      
        //Initialize current computer's basic information
        ComputerInfo computerOriginal;
        computerOriginal.Update();

        //Serialize the information
        std::ostringstream computerInfoStream;
        boost::archive::text_oarchive computerInfoArchiver(computerInfoStream);
        computerInfoArchiver << computerOriginal;

        //Send the whole information to client
        std::string tempString = computerInfoStream.str();
        const char* sendBuffer = tempString.c_str();
        size_t sendBufferLen = computerInfoStream.str().length();

        if (SOCKET_ERROR == Send(senderSock, (char*)&sendBufferLen, sizeof(sendBufferLen)))
        {
            return EXIT_FAILURE;
        }

        if (SOCKET_ERROR == Send(senderSock, sendBuffer, sendBufferLen))
        {
            return EXIT_FAILURE;
        }
    }

    closesocket(senderSock);

    system("pause");
    return 0;
}


