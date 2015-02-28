#ifndef CORE_UTILITY_APPLICATION_MANAGERSERVER_MANAGERSERVER_H__INCLUDED
#define CORE_UTILITY_APPLICATION_MANAGERSERVER_MANAGERSERVER_H__INCLUDED

#include "Singleton.h"
#include "Network/ThreadServer.h"
#include "MsgDef.h"
#include "XString/XString.h"

using Core::Utility::CThreadServer;
using Core::Utility::TSingleton;
using Core::Utility::XString;
using Core::Utility::SMsgDynamic;

class CManagerServer : public CThreadServer
{
public:
	CManagerServer();
	virtual ~CManagerServer();

	void Init();
	virtual void Process();
	virtual void ProcessCommand();
	virtual void OnCommand(const XString& strCmd, const std::vector<XString>& params);

	void OnAckLoginCheck(MsgAckLoginCheck* ackMsg);
	void OnAckPlayerEnterWorld(DBMsgAckEnterWorld* ackMsg);
	void OnUpdatePlayerDynamicInfo(SMsgDynamic* pMsg);
};

#endif