#ifndef CORE_UTILITY_APPLICATION_GATESERVER_MANAGERCONNECTOR_H__INCLUDED
#define CORE_UTILITY_APPLICATION_GATESERVER_MANAGERCONNECTOR_H__INCLUDED

#include "Network/Worker/NetworkConnector.h"

using Core::Utility::CNetworkConnector;

class CManagerServerConnector : public CNetworkConnector
{
public:
	CManagerServerConnector();
	virtual ~CManagerServerConnector();

	virtual void ProcessMsg();

};

#endif