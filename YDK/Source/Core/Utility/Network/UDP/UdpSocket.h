#ifndef CORE_UTILITY_NETWORK_UDP_UDPSOCKET_H__INCLUDED
#define CORE_UTILITY_NETWORK_UDP_UDPSOCKET_H__INCLUDED

#include "Network/Socket.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CUdpSocket : public CSocket
{
public:
	CUdpSocket();
	virtual ~CUdpSocket();

	virtual bool Create();
	virtual int32 SendTo(CSockAddr& addr, const char* pBuff, uint32 uiLength, int32 iFlag = 0);
	virtual int32 RecvFrom(CSockAddr& addr, char* pBuff, uint32 uiLength, int32 uiFlag = 0);
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif