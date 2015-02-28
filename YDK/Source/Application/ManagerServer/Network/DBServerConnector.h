#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_DBSERVERCONNECTOR_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_DBSERVERCONNECTOR_H__INCLUDED

#include "Network/Worker/NetworkConnector.h"

using Core::Utility::CNetworkConnector;

class CDBServerConnector : public CNetworkConnector
{
public:
	CDBServerConnector();
	virtual ~CDBServerConnector();

	virtual void ProcessMsg();

};

#endif