#include <iostream>
#include <iterator>
#include <ctime>

#include "ClientCMD.h"
#include "Network/ServerConnector.h"
#include "MsgDef.h"
#include "ClientPlayer.h"
#include "CommonDataStruct/ChatData_Struct.h"
#include "ChatHistoryManager.h"
#include "msini/CMSINIReader.h"
#include "FileTransManager.h"

using Core::Utility::CMSINIReader;

CClientCMD::CClientCMD()
{
}

CClientCMD::~CClientCMD()
{
}

void CClientCMD::Init()
{
	//Load Configuration File
	TSingleton<CMSINIReader>::GetInstance().SetFileName(TEXT(".\\ClientConfig.ini"));
	XString gateServerIP = TSingleton<CMSINIReader>::GetInstance().GetFieldStringValue(TEXT("Client"), TEXT("GateServerIP"));
	int32 gateServerPort = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("Client"), TEXT("GateServerPort"));

	TSingleton<CServerConnector>::GetInstance().Init(gateServerIP, gateServerPort);
	TSingleton<CChatHistoryManager>::GetInstance().LoadHistory();
}

void CClientCMD::Process()
{
	TSingleton<CServerConnector>::GetInstance().ProcessMsg();
}

void CClientCMD::ProcessCommand()
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

void CClientCMD::OnCommand( const XString& strCmd, std::vector<XString>& params )
{
	if (strCmd == TEXT("test"))
	{
		std::cout << TEXT("test succeeded") << std::endl;
	}
	else if (strCmd == TEXT("sendbigpacket"))
	{
		MsgTestBigPacket* msg = new MsgTestBigPacket;
		XString str(2* 8195 - 10, 'f');
		strcpy_s(msg->content, str.c_str());
		TSingleton<CServerConnector>::GetInstance().SendMsg(msg);
	}
	else if (strCmd == TEXT("login"))
	{
		if (params.size() >= 2)
		{
			MsgReqLoginCheck msgReqLoginCheck;
			strcpy_s(msgReqLoginCheck.account, params[0].c_str());
			strcpy_s(msgReqLoginCheck.password, params[1].c_str());

			TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqLoginCheck);
		}
	}
	else if (strCmd == TEXT("enterworld"))
	{
		MsgReqEnterWorld msgReqEnterWorld;
		msgReqEnterWorld.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());

		TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqEnterWorld);
	}
	else if (strCmd == TEXT("chatsingle"))
	{
		if (params.size() >= 2)
		{
			uint32 unSenderID = TSingleton<CClientPlayer>::GetInstance().GetAccountID();

			SChatData chatData;
			chatData.m_unTargetID = params[0].ConvertToUInt32();
			chatData.m_unChannel = EChannel_Single;
			chatData.m_strContent = params[1];
			chatData.m_unSenderID = unSenderID;
			chatData.m_strSenderName = TSingleton<CClientPlayer>::GetInstance().GetNickName();
			chatData.m_unTime = static_cast<uint32>(::time(NULL));

			CDynamicPacket cPacket(EWorldMsg_Chat);
			cPacket.GetMsg()->SetPlayerID(unSenderID);
			cPacket << chatData;
			TSingleton<CServerConnector>::GetInstance().SendMsg(cPacket.GetMsg());
			TSingleton<CChatHistoryManager>::GetInstance().Save(chatData, true);
		}
	}
	else if (strCmd == TEXT("chatgroup"))
	{
		if (params.size() >= 2)
		{
			uint32 unSenderID = TSingleton<CClientPlayer>::GetInstance().GetAccountID();

			SChatData chatData;
			chatData.m_unTargetID = params[0].ConvertToUInt32();
			chatData.m_unChannel = EChannel_Group;
			chatData.m_strContent = params[1];
			chatData.m_unSenderID = unSenderID;
			chatData.m_strSenderName = TSingleton<CClientPlayer>::GetInstance().GetNickName();
			chatData.m_unTime = static_cast<uint32>(::time(NULL));

			CDynamicPacket cPacket(EWorldMsg_Chat);
			cPacket.GetMsg()->SetPlayerID(unSenderID);
			cPacket << chatData;
			TSingleton<CServerConnector>::GetInstance().SendMsg(cPacket.GetMsg());
			TSingleton<CChatHistoryManager>::GetInstance().Save(chatData, true);
		}
	}
	else if (strCmd == TEXT("addfriend"))
	{
		if (params.size() >= 1)
		{
			MsgReqAddRelation reqMsg;
			reqMsg.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
			strcpy_s(reqMsg.m_targetAccount, params[0].c_str());
			TSingleton<CServerConnector>::GetInstance().SendMsg(&reqMsg);
		}
	}
	else if (strCmd == TEXT("delfriend"))
	{
		if (params.size() >= 1)
		{
			MsgReqDelRelation reqMsg;
			reqMsg.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
			reqMsg.m_unPlayerID = params[0].ConvertToInt32();
			TSingleton<CServerConnector>::GetInstance().SendMsg(&reqMsg);
		}
	}
	else if (strCmd == TEXT("showfriendlist"))
	{
		SRelation& friends = TSingleton<CClientPlayer>::GetInstance().GetRelation();
		std::map<uint32, SRelationData>::iterator iter = friends.m_mapRelations.begin();
		for (; iter != friends.m_mapRelations.end(); ++iter)
		{
			Core::LOG(XString().Format(TEXT("ID:%d,Nickname:%s,state:%s")
										, iter->second.m_unCharactorID
										, iter->second.m_strNickname.c_str()
										, iter->second.m_bIsOnline ? TEXT("online") : TEXT("offline")).c_str());
		}
	}
	else if (strCmd == TEXT("exitworld"))
	{
		MsgReqExitWorld msgReqExitWorld;
		msgReqExitWorld.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());

		TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqExitWorld);
	}
	else if (strCmd == TEXT("creategroup"))
	{
		if (params.size() >= 1)
		{
			MsgReqCreateGroup msgReqCreateGroup;
			msgReqCreateGroup.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
			strcpy_s(msgReqCreateGroup.m_groupName, params[0].c_str());
			TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqCreateGroup);
		}
	}
	else if (strCmd == TEXT("showgroups"))
	{
		SGroups groups = TSingleton<CClientPlayer>::GetInstance().GetGroups();
		SGroups::MAPGROUPS::iterator groupIter = groups.m_mapGroups.begin();
		for (; groupIter != groups.m_mapGroups.end(); ++groupIter)
		{
			Core::LOG(XString().Format(TEXT("GroupID:%d, Groupname:%s"), groupIter->first, groupIter->second.m_strGroupName.c_str()).c_str());
			SGroupData::MAPGROUPMEMBERS::iterator memberIter = groupIter->second.m_mapMembers.begin();
			for (; memberIter != groupIter->second.m_mapMembers.end(); ++memberIter)
			{
				Core::LOG(XString().Format(TEXT("MemberID:%d, MemberName:%s, MemberStatus:%s")
					, memberIter->first
					, memberIter->second.m_strName.c_str()
					, memberIter->second.m_bIsOnline ? TEXT("online") : TEXT("offline")).c_str());
			}
		}
	}
	else if (strCmd == TEXT("joingroup"))
	{
		if (params.size() >= 1)
		{
			MsgReqJoinGroup reqJoinGroup;
			reqJoinGroup.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
			reqJoinGroup.m_unGroupID = params[0].ConvertToUInt32();
			TSingleton<CServerConnector>::GetInstance().SendMsg(&reqJoinGroup);
		}
	}
	else if (strCmd == TEXT("leavegroup"))
	{
		if (params.size() >= 1)
		{
			MsgReqLeaveGroup reqLeaveGroup;
			reqLeaveGroup.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
			reqLeaveGroup.m_unGroupID = params[0].ConvertToUInt32();
			TSingleton<CServerConnector>::GetInstance().SendMsg(&reqLeaveGroup);
		}
	}
	else if (strCmd == TEXT("showhistory"))
	{
		if (params.size() >= 1)
		{
			uint32 friendID = params[0].ConvertToUInt32();
			CChatHistoryManager::LSTHISTORYLIST* pHistoryDataList = TSingleton<CChatHistoryManager>::GetInstance().GetPlayerHistory(friendID);
			if (pHistoryDataList != NULL)
			{
				CChatHistoryManager::LSTHISTORYLIST::iterator iter = pHistoryDataList->begin();
				for (; iter != pHistoryDataList->end(); ++iter)
				{
					Core::LOG(XString().Format(TEXT("%s:%s")
										, iter->m_strSenderName.c_str()
										, iter->m_strContent.c_str()).c_str());
				}
			}
		}
	}
	else if (strCmd == TEXT("sendfile"))
	{
		if (params.size() >= 1)
		{
			uint32 friendID = params[0].ConvertToUInt32();
			XString filePath = TEXT("F:\\VA_X_Setup1862.exe");

			TSingleton<CFileTransManager>::GetInstance().SendFileReq(friendID, filePath);
		}
	}
	else if (strCmd == TEXT("register"))
	{
		if (params.size() >= 3)
		{
			XString strAccount = params[0];
			XString strPassword = params[1];
			XString strNickname = params[2];

			MsgReqRegisterAccount reqMsg;
			strcpy_s(reqMsg.m_account, strAccount.c_str());
			strcpy_s(reqMsg.m_password, strPassword.c_str());
			strcpy_s(reqMsg.m_nickname, strNickname.c_str());

			TSingleton<CServerConnector>::GetInstance().SendMsg(&reqMsg);
		}
	}
}