#include <iterator>
#include <ctime>

#include "ClientGUI/ClientGUI.h"
#include "Network/ServerConnector.h"
#include "ClientGUI/ClientPlayer.h"
#include "ClientGUI/ChatHistoryManager.h"
#include "CommonDataStruct/ChatData_Struct.h"
#include "msini/CMSINIReader.h"
#include "MsgDef.h"
#include "UI/MainFrame.h"
#include "UI/LoginFrame.h"
#include "UI/RegisterFrame.h"

using Core::Utility::CMSINIReader;

CClientGUI::CClientGUI()
{
}

CClientGUI::~CClientGUI()
{
}

void CClientGUI::Init()
{
	//Load Configuration File
	TSingleton<CMSINIReader>::GetInstance().SetFileName(TEXT(".\\ClientConfig.ini"));
	XString gateServerIP = TSingleton<CMSINIReader>::GetInstance().GetFieldStringValue(TEXT("Client"), TEXT("GateServerIP"));
	int32 gateServerPort = TSingleton<CMSINIReader>::GetInstance().GetFieldIntValue(TEXT("Client"), TEXT("GateServerPort"));

	TSingleton<CServerConnector>::GetInstance().Init(gateServerIP, gateServerPort);
	TSingleton<CChatHistoryManager>::GetInstance().LoadHistory();
}

void CClientGUI::Process()
{
	TSingleton<CServerConnector>::GetInstance().ProcessMsg();
}

void CClientGUI::OnReqLogin( const XString& strAccount, const XString& strPassword )
{
	MsgReqLoginCheck msgReqLoginCheck;
	strcpy_s(msgReqLoginCheck.account, strAccount.c_str());
	strcpy_s(msgReqLoginCheck.password, strPassword.c_str());

	TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqLoginCheck);
}

bool CClientGUI::OnAckLogin( MsgAckLoginCheck* pMsg )
{
	bool bRet = true;
	if (pMsg != NULL)
	{
		if (pMsg->result == MsgAckLoginCheck::EResult_Success)
		{
			TSingleton<CClientPlayer>::GetInstance().SetAccountID(pMsg->accountID);
		}
		else if (pMsg->result == MsgAckLoginCheck::EResult_IncorrectAccOrPwd)
		{
			wxMessageBox(_T("密码或者账号错误"));
			bRet = false;
		}
	}
	
	return bRet;
}

void CClientGUI::OnReqEnterWorld()
{
	MsgReqEnterWorld msgReqEnterWorld;
	msgReqEnterWorld.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());

	TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqEnterWorld);
}

void CClientGUI::OnAckEnterWorld( MsgAckEnterWorld* pMsg )
{
	uint8 result = pMsg->m_result;
	if (result == MsgAckEnterWorld::EResult_PlayerAlreadyInWorld)
	{
		wxMessageBox(_T("已经登录"));
	}
	else if (result == MsgAckEnterWorld::EResult_PlayerNotFound)
	{
		wxMessageBox(_T("找不到角色"));
	}
	else if (result == MsgAckEnterWorld::EResult_Success)
	{
		TSingleton<CClientPlayer>::GetInstance().SetNickName(pMsg->m_playerBaseInfo.m_nickName);
		Core::LOG(XString().Format("Player %s Enter World", TSingleton<CClientPlayer>::GetInstance().GetNickName().c_str()).c_str());

		wxCommandEvent evt(EVT_COMMAND_CREATE_MAIN_FRAME, ID_LoginFrame);
		evt.SetString(TSingleton<CClientPlayer>::GetInstance().GetNickName().c_str());
		wxPostEvent(&(TSingleton<CLoginFrame>::GetInstance()), evt);
	}
}

void CClientGUI::OnAckInitRelation( SMsgDynamic* pMsg )
{
	CDynamicPacket cPacket(pMsg);
	if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
	{
		SRelation relation;
		relation.SerializeClient(cPacket);
		TSingleton<CClientPlayer>::GetInstance().SetRelation(relation);

		wxCommandEvent evt(EVT_COMMAND_INIT_RELATION_TREE, ID_FriendsTree);
		wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);
	}
}

void CClientGUI::OnAckUpdateRelation( SMsgDynamic* pMsg )
{
	CDynamicPacket cPacket(pMsg);
	if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
	{
		uint8 oper = 0;
		SRelationData relationData;
		cPacket << oper;
		relationData.SerializeClient(cPacket);

		SRelation& playerRealtion = TSingleton<CClientPlayer>::GetInstance().GetRelation();
		if (oper == SRelation::EOPER_AddRelation)
		{
			playerRealtion.AddRelation(relationData);
		}
		else if (oper == SRelation::EOPER_DelRelation)
		{
			playerRealtion.DelRelation(relationData);
		}

		wxCommandEvent evt(EVT_COMMAND_UPDATE_RELATION_TREE, ID_FriendsTree);
		evt.SetInt(relationData.m_unCharactorID);
		wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);
	}
}

void CClientGUI::OnAckUpdateFriendGroup( SMsgDynamic* pMsg )
{
	CDynamicPacket cPacket(pMsg);
	if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
	{
		uint8 oper = 0;
		SFriendGroupData friendGroupData;
		cPacket << oper;
		cPacket << friendGroupData;

		SRelation& playerRealtion = TSingleton<CClientPlayer>::GetInstance().GetRelation();
		if (oper == SRelation::EOPER_AddFriendGroup)
		{
			playerRealtion.AddFriendGroup(friendGroupData);
		}
		else if (oper == SRelation::EOPER_DelFriendGroup)
		{
			playerRealtion.DelFriendGroup(friendGroupData);
		}

		wxCommandEvent evt(EVT_COMMAND_UPDATE_FRIEND_GROUP_ROOT, ID_FriendsTree);
		evt.SetInt(friendGroupData.m_unFriendGroupID);
		wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);
	}
}

void CClientGUI::OnSendSingleChat( uint32 targetID, const XString& strContent, time_t sendTime )
{
	uint32 unSenderID = TSingleton<CClientPlayer>::GetInstance().GetAccountID();

	SChatData chatData;
	chatData.m_unTargetID = targetID;
	chatData.m_unChannel = EChannel_Single;
	chatData.m_strContent = strContent;
	chatData.m_unSenderID = unSenderID;
	chatData.m_strSenderName = TSingleton<CClientPlayer>::GetInstance().GetNickName();
	chatData.m_unTime = static_cast<uint32>(sendTime);

	CDynamicPacket cPacket(EWorldMsg_Chat);
	cPacket.GetMsg()->SetPlayerID(unSenderID);
	cPacket << chatData;
	TSingleton<CServerConnector>::GetInstance().SendMsg(cPacket.GetMsg());
	TSingleton<CChatHistoryManager>::GetInstance().Save(chatData, true);
}

void CClientGUI::OnRecvSingleChat(SChatData* pChatData)
{
	wxCommandEvent evt(EVT_COMMAND_RECV_SINGLE_CHATMSG, ID_FriendsTree);
	evt.SetInt(pChatData->m_unSenderID);
	evt.SetString(pChatData->m_strContent.c_str());
	evt.SetTimestamp(pChatData->m_unTime);
	wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);

	//Core::LOG(XString().Format("Player %s:%s", chatData.m_strSenderName.c_str(), chatData.m_strContent.c_str()).c_str());
	TSingleton<CChatHistoryManager>::GetInstance().Save(*pChatData, false);
}

void CClientGUI::OnAddFriend( const XString& strAccount )
{
	MsgReqAddRelation reqMsg;
	reqMsg.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
	strcpy_s(reqMsg.m_targetAccount, strAccount.c_str());
	TSingleton<CServerConnector>::GetInstance().SendMsg(&reqMsg);
}

void CClientGUI::OnDelFriend( uint32 unPlayerID)
{
	MsgReqDelRelation reqMsg;
	reqMsg.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
	reqMsg.m_unPlayerID = unPlayerID;
	TSingleton<CServerConnector>::GetInstance().SendMsg(&reqMsg);
}

void CClientGUI::OnAckInitGroups( SMsgDynamic* pMsg )
{
	CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
	if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
	{
		SGroups groups;
		groups.SerializeClient(cPacket);
		TSingleton<CClientPlayer>::GetInstance().SetGroups(groups);
		//Core::LOG("Player Groups Inited");

		wxCommandEvent evt(EVT_COMMAND_INIT_GROUP_TREE, ID_GroupsTree);
		wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);
	}
}

void CClientGUI::OnAckUpdateGroup( SMsgDynamic* pMsg )
{
	CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
	if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
	{
		uint8 oper = 0;
		SGroupData groupData;
		cPacket << oper;
		groupData.SerializeClient(cPacket);

		SGroups& groups = TSingleton<CClientPlayer>::GetInstance().GetGroups();
		if (oper == SGroups::EOPER_AddGroup)
		{
			groups.AddGroup(groupData);
		}
		else
		{
			groups.DelGroup(groupData);
		}

		wxCommandEvent evt(EVT_COMMAND_UPDATE_GROUP_TREE, ID_GroupsTree);
		evt.SetInt(groupData.m_unGroupID);
		wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);
	}
}

void CClientGUI::OnAckUpdateGroupMember( SMsgDynamic* pMsg )
{
	CDynamicPacket cPacket(static_cast<SMsgDynamic*>(pMsg));
	if (pMsg->GetPlayerID() == TSingleton<CClientPlayer>::GetInstance().GetAccountID())
	{
		uint8 oper = 0;
		uint32 groupID = 0;
		SGroupMemeber groupMemeber;
		cPacket << oper << groupID;
		groupMemeber.SerializeClient(cPacket);

		SGroups& groups = TSingleton<CClientPlayer>::GetInstance().GetGroups();
		SGroupData* pGroup = groups.GetGroup(groupID);
		if (pGroup != NULL)
		{
			if (oper == SGroupData::EOPER_JoinGroup)
			{
				pGroup->JoinGroup(groupMemeber);
			}
			else
			{
				pGroup->LeaveGroup(groupMemeber);
			}
		}

		wxCommandEvent evt(EVT_COMMAND_UPDATE_GROUP_MEMBER, ID_MainFrame);
		evt.SetInt(groupID);
		evt.SetExtraLong(groupMemeber.m_unMemeberID);
		wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);
	}
}

void CClientGUI::OnAddGroup( const XString& strAccount )
{
	MsgReqJoinGroup reqJoinGroup;
	reqJoinGroup.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
	reqJoinGroup.m_unGroupID = const_cast<XString&>(strAccount).ConvertToUInt32();
	TSingleton<CServerConnector>::GetInstance().SendMsg(&reqJoinGroup);
}

void CClientGUI::OnDelGroup( uint32 unGroupID )
{
	MsgReqLeaveGroup reqLeaveGroup;
	reqLeaveGroup.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
	reqLeaveGroup.m_unGroupID = unGroupID;
	TSingleton<CServerConnector>::GetInstance().SendMsg(&reqLeaveGroup);
}

void CClientGUI::OnCreateGroup( const XString& strName )
{
	MsgReqCreateGroup msgReqCreateGroup;
	msgReqCreateGroup.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
	strcpy_s(msgReqCreateGroup.m_groupName, strName.c_str());
	TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqCreateGroup);
}

void CClientGUI::OnCreateFriendGroup( const XString& strName )
{
	MsgReqCreateFriendGroup msgReqCreateFriendGroup;
	msgReqCreateFriendGroup.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
	strcpy_s(msgReqCreateFriendGroup.m_groupName, strName.c_str());
	TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqCreateFriendGroup);
}

void CClientGUI::OnReqChangeFriendGroup( uint32 unPlayerID, uint32 unFriendGroupID )
{
	MsgReqChangeCustomGroup msgReqChangeCustomGroup;
	msgReqChangeCustomGroup.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
	msgReqChangeCustomGroup.m_unFriendID = unPlayerID;
	msgReqChangeCustomGroup.m_unNewCustomGroupID = unFriendGroupID;
	TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqChangeCustomGroup);
}

void CClientGUI::OnReqDeleteFriendGroup( uint32 unFriendGroupID )
{
	MsgReqDelCustomGroup msgReqDelCustomGroup;
	msgReqDelCustomGroup.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());
	msgReqDelCustomGroup.m_unCustomGroupID = unFriendGroupID;
	TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqDelCustomGroup);
}

void CClientGUI::OnReqExitWorld()
{
	MsgReqExitWorld msgReqExitWorld;
	msgReqExitWorld.SetPlayerID(TSingleton<CClientPlayer>::GetInstance().GetAccountID());

	TSingleton<CServerConnector>::GetInstance().SendMsg(&msgReqExitWorld);
}

void CClientGUI::OnSendGroupChat( uint32 targetID, const XString& strContent, time_t sendTime )
{
	uint32 unSenderID = TSingleton<CClientPlayer>::GetInstance().GetAccountID();

	SChatData chatData;
	chatData.m_unTargetID = targetID;
	chatData.m_unChannel = EChannel_Group;
	chatData.m_strContent = strContent;
	chatData.m_unSenderID = unSenderID;
	chatData.m_strSenderName = TSingleton<CClientPlayer>::GetInstance().GetNickName();
	chatData.m_unTime = static_cast<uint32>(sendTime);

	CDynamicPacket cPacket(EWorldMsg_Chat);
	cPacket.GetMsg()->SetPlayerID(unSenderID);
	cPacket << chatData;
	TSingleton<CServerConnector>::GetInstance().SendMsg(cPacket.GetMsg());
	//TSingleton<CChatHistoryManager>::GetInstance().Save(chatData, true);
}

void CClientGUI::OnRecvGroupChat( SChatData* pChatData)
{
	wxCommandEvent evt(EVT_COMMAND_RECV_GROUP_CHATMSG, ID_GroupsTree);
	evt.SetInt(pChatData->m_unSenderID);
	evt.SetExtraLong(pChatData->m_unTargetID);
	evt.SetString(pChatData->m_strContent.c_str());
	evt.SetTimestamp(pChatData->m_unTime);
	wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);

	//Core::LOG(XString().Format("Player %s:%s", chatData.m_strSenderName.c_str(), chatData.m_strContent.c_str()).c_str());
	TSingleton<CChatHistoryManager>::GetInstance().Save(*pChatData, false);
}

void CClientGUI::OnReqRegister(const XString& strAccount, const XString& strPassword, const XString& strNickName)
{
	MsgReqRegisterAccount reqMsg;
	strcpy_s(reqMsg.m_account, strAccount.c_str());
	strcpy_s(reqMsg.m_password, strPassword.c_str());
	strcpy_s(reqMsg.m_nickname, strNickName.c_str());

	TSingleton<CServerConnector>::GetInstance().SendMsg(&reqMsg);
}

void CClientGUI::OnAckRegister( MsgAckRegisterAccount* pMsg )
{
	MsgAckRegisterAccount* ackMsg = static_cast<MsgAckRegisterAccount*>(pMsg);
	if (ackMsg != NULL && ackMsg->m_result == MsgAckRegisterAccount::EResult_Success)
	{
		Core::LOG("Register Success");
	}
	else
	{
		Core::LOG("Register Failed");
	}

	wxCommandEvent evt(EVT_COMMAND_RECV_REGISTER_RESULT, ID_Register_Frame);
	evt.SetInt(ackMsg->m_result);
	wxPostEvent(&(TSingleton<CRegisterFrame>::GetInstance()), evt);
}


	