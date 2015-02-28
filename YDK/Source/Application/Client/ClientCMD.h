#ifndef CORE_UTILITY_APPLICATION_CLIENT_CLIENTCMD_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENT_CLIENTCMD_H__INCLUDED

#include "Singleton.h"
#include "Network/ThreadServer.h"

using Core::Utility::CThreadServer;
using Core::Utility::TSingleton;
using Core::Utility::XString;

class CClientCMD : public CThreadServer
{
public:
	CClientCMD();
	virtual ~CClientCMD();

	void Init();
	virtual void Process();

protected:
	virtual void ProcessCommand();
	virtual void OnCommand(const XString& strCmd, std::vector<XString>& params);
};


#endif