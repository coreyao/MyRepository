#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_SERVERCONNECTOR_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_SERVERCONNECTOR_H__INCLUDED

#include "Network/Worker/NetworkConnector.h"

using Core::Utility::CNetworkConnector;

class CServerConnector : public CNetworkConnector
{
public:
	CServerConnector();
	virtual ~CServerConnector();

	virtual void ProcessMsg();
};

#endif