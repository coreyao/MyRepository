#include <iostream>
#include <iterator>

#include "Network/GateServerListener.h"
#include "Network/DBServerConnector.h"
#include "AccountManager/AccountManager.h"
#include "CharactorManager/CharactorManager.h"
#include "GroupManager/GroupManager.h"
#include "ManagerServer.h"
#include "msini/CMSINIReader.h"

using Core::Utility::TSingleton;
using Core::Utility::CMSINIReader;

CManagerServer::CManagerServer()
{
}

CManagerServer::~CManagerServer()
{
}

void CManagerServer::Init()
{
	//Load Configuration File
	TSingleton<CMSINIReader>::GetInstance().SetFileName(TEXT(".\\ServerConfig.ini"));

	XString managerServerIP = TSingleton<CMSINIReader>::GetInstance().GetFieldStringValue(TEXT("ManagerServer"), TEXT("ManagerServerIP"));
	int32 managerServerPort = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("ManagerServer"), TEXT("ManagerServerPort"));
	uint32 managerServerMaxConnections = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("ManagerServer"), TEXT("ManagerServerMaxConnections"));

	XString DBServerIP = TSingleton<CMSINIReader>::GetInstance().GetFieldStringValue(TEXT("DBServer"), TEXT("DBServerIP"));
	int32 DBServerPort = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("DBServer"), TEXT("DBServerPort"));

	TSingleton<CGateServerListener>::GetInstance().Init(managerServerPort, managerServerMaxConnections, managerServerIP);
	TSingleton<CGateServerListener>::GetInstance().Start();

	TSingleton<CDBServerConnector>::GetInstance().Init(DBServerIP, static_cast<uint16>(DBServerPort));

	TSingleton<CGroupManager>::GetInstance().Init();
}

void CManagerServer::Process()
{
	TSingleton<CGateServerListener>::GetInstance().ProcessMsg();
	TSingleton<CDBServerConnector>::GetInstance().ProcessMsg();

	CGameObject::UpdateTimerObject();
}

void CManagerServer::OnAckLoginCheck( MsgAckLoginCheck* ackMsg )
{
	if (ackMsg->result == MsgAckLoginCheck::EResult_Success)
	{
		TSingleton<CAccountManager>::GetInstance().OnJoin(ackMsg->accountID, ackMsg->account, ackMsg->gateID, ackMsg->challengeID);
	}

	TSingleton<CGateServerListener>::GetInstance().SendMsg(ackMsg, ackMsg->gateID);
}

void CManagerServer::ProcessCommand()
{
	XString lineStr;
	while (getline(std::cin, lineStr))
	{
		std::vector<XString> strs = lineStr.SplitString();
		if (strs.size() == 1)
		{
			OnCommand(strs[0], std::vector<XString>());
		}
		else
		{
			std::vector<XString> params;
			copy(strs.begin() + 1, strs.end(), back_inserter(params));
			OnCommand(strs[0], params);
		}
	}
}

void CManagerServer::OnCommand( const XString& strCmd, const std::vector<XString>& params )
{
	if (strCmd == "test")
	{ 
		MsgAckLoginCheck ackMsg;
		ackMsg.gateID = 1;
		TSingleton<CGateServerListener>::GetInstance().SendMsg(&ackMsg, ackMsg.gateID);
	}
}

void CManagerServer::OnAckPlayerEnterWorld( DBMsgAckEnterWorld* ackMsg )
{
	TSingleton<CCharactorManager>::GetInstance().OnPlayerEnterWorld(ackMsg);
}
