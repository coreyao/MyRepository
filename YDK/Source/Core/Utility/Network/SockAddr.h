#ifndef CORE_UTILITY_NETWORK_SOCKADDR_H__INCLUDED
#define CORE_UTILITY_NETWORK_SOCKADDR_H__INCLUDED

#include "Network/SocketPlatform.h"
#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

using Core::Utility::XChar;
using Core::Utility::XString;

class CSockAddr
{
public:
	CSockAddr(uint16 usPort = 0);
	CSockAddr(const CSockAddr& addr);
	CSockAddr(const struct sockaddr* pAddr);
	CSockAddr(const XString& strIP, uint16 usPort);

	virtual ~CSockAddr();

	XString ToString(bool bIncludePort = true) const;
	uint16 GetPort() const;
	uint32 GetIP() const;
	uint32 GetSockAddrLength() const;
	int32 GetFamily() const;
	void SetFamily(short family);
	void Clear();
	void SetAddress(const XString& strIP);
	void SetAddrPort(uint16 usPort);

	operator struct sockaddr* ();
	operator struct sockaddr_in* ();
	CSockAddr& operator=(const CSockAddr& addr);
	CSockAddr& operator=(const sockaddr* pAddr);
	bool operator==(const CSockAddr& addr) const;

protected:
	struct sockaddr_in m_addr;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)


#endif