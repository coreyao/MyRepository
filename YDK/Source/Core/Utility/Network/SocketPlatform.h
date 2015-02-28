#ifndef CORE_UTILITY_NETWORK_SOCKETPLATFORM_H__INCLUDED
#define CORE_UTILITY_NETWORK_SOCKETPLATFORM_H__INCLUDED

#include "BasicDefines.h"
#include "UtilityTypes.h"

#include <WinSock2.h>

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

const int32 ConUserDefineError = -1;

#ifdef _WIN32

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

class WSAInitializer
{
public:
	WSAInitializer()
	{
		if (::WSAStartup(MAKEWORD(2, 2), &m_wsaData))
		{
			::exit(-1);
		}
	}

	~WSAInitializer()
	{
		::WSACleanup();
	}

private:
	WSADATA m_wsaData;
};

#endif

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif
