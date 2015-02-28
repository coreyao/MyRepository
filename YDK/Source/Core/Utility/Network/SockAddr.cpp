#include "Network/SockAddr.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CSockAddr::CSockAddr(uint16 usPort/* = 0*/)
{
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(usPort);
}

CSockAddr::CSockAddr(const CSockAddr& addr)
{
	memcpy(&m_addr, &(addr.m_addr), sizeof(struct sockaddr_in));
}

CSockAddr::CSockAddr(const struct sockaddr* pAddr)
{
	memcpy(&m_addr, pAddr, sizeof(struct sockaddr));
}

CSockAddr::CSockAddr(const Core::Utility::XString &strIP, Core::Utility::uint16 usPort)
{
	memset(&m_addr, 0, sizeof(sockaddr_in));

	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(usPort);
	m_addr.sin_addr.S_un.S_addr = inet_addr(strIP.c_str());
}

CSockAddr::~CSockAddr()
{
}

XString CSockAddr::ToString(bool bIncludePort/* = true*/) const
{
	XString result;
	XChar* strIP = inet_ntoa(m_addr.sin_addr);
	if (bIncludePort)
	{
		result.Format("%s:%d", strIP, ntohs(m_addr.sin_port));
	}
	else
	{
		result.Format("%s", strIP);
	}

	return result;
}

uint16 CSockAddr::GetPort() const
{
	return ntohs(m_addr.sin_port);
}

uint32 CSockAddr::GetIP() const
{
	return m_addr.sin_addr.S_un.S_addr;
}

uint32 CSockAddr::GetSockAddrLength() const
{
	return sizeof(m_addr);
}

int32 CSockAddr::GetFamily() const
{
	return m_addr.sin_family;
}

void CSockAddr::SetFamily(short family)
{
	m_addr.sin_family = family;
}

void CSockAddr::Clear()
{
	memset(&m_addr, 0, sizeof(m_addr));
}

void CSockAddr::SetAddress(const XString& strIP)
{
	if (strIP.empty())
	{
		m_addr.sin_addr.S_un.S_addr = ADDR_ANY;
	}
	else
	{
		m_addr.sin_addr.S_un.S_addr = inet_addr(strIP.c_str());
	}
}

void CSockAddr::SetAddrPort(uint16 usPort)
{
	m_addr.sin_port = htons(usPort);
}

CSockAddr::operator struct sockaddr* ()
{
	return reinterpret_cast<struct sockaddr*>(&m_addr);
};

CSockAddr::operator struct sockaddr_in* ()
{
	return &m_addr;
};

CSockAddr& CSockAddr::operator=(const CSockAddr& addr)
{
	if (&addr != this)
	{
		memcpy(&m_addr, &(addr.m_addr), sizeof(CSockAddr));
	}

	return *this;
}

CSockAddr& CSockAddr::operator=(const sockaddr* pAddr)
{
	memcpy(&m_addr, pAddr, sizeof(sockaddr));

	return *this;
}

bool CSockAddr::operator==(const CSockAddr& addr) const
{
	if (m_addr.sin_family == addr.m_addr.sin_family
	&&  m_addr.sin_port == addr.m_addr.sin_port
	&&  m_addr.sin_addr.S_un.S_addr == addr.m_addr.sin_addr.S_un.S_addr)
	{
		return true;
	}

	return false;
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

