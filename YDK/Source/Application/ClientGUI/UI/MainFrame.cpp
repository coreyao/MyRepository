#include "UI/MainFrame.h"
#include "ClientGUI/ClientPlayer.h"
#include "Singleton.h"
#include "UI/FriendChatFrame.h"
#include "UI/FriendAddFrame.h"
#include "UI/GroupAddFrame.h"
#include "UI/GroupCreateFrame.h"
#include "UI/LoginFrame.h"
#include "UI/GroupChatFrame.h"
#include "UI/FileGuageFrame.h"
#include "UI/FriendGroupCreateFrame.h"
#include "ClientGUI/ClientGUI.h"
#include "ClientGUI/FileTransManager.h"

using Core::Utility::TSingleton;

const uint32 conKB = 1024;
const uint32 conMB = conKB * 1024;
const uint32 conGB = conMB * 1024;

DEFINE_EVENT_TYPE(EVT_COMMAND_INIT_RELATION_TREE)
DEFINE_EVENT_TYPE(EVT_COMMAND_UPDATE_RELATION_TREE)
DEFINE_EVENT_TYPE(EVT_COMMAND_UPDATE_FRIEND_GROUP_ROOT)
DEFINE_EVENT_TYPE(EVT_COMMAND_RECV_SINGLE_CHATMSG)
DEFINE_EVENT_TYPE(EVT_COMMAND_CLOSE_FRIEND_CHATFRAME)
DEFINE_EVENT_TYPE(EVT_COMMAND_PROMOTE_FILE_SEND)
DEFINE_EVENT_TYPE(EVT_COMMAND_SHOW_PERCENT)

DEFINE_EVENT_TYPE(EVT_COMMAND_INIT_GROUP_TREE)
DEFINE_EVENT_TYPE(EVT_COMMAND_UPDATE_GROUP_TREE)
DEFINE_EVENT_TYPE(EVT_COMMAND_UPDATE_GROUP_MEMBER)
DEFINE_EVENT_TYPE(EVT_COMMAND_RECV_GROUP_CHATMSG)
DEFINE_EVENT_TYPE(EVT_COMMAND_CLOSE_GROUP_CHATFRAME)

BEGIN_EVENT_TABLE(CMainFrame, wxFrame)
	EVT_COMMAND(ID_FriendsTree, EVT_COMMAND_INIT_RELATION_TREE, CMainFrame::OnInitFriendsTree)
	EVT_COMMAND(ID_FriendsTree, EVT_COMMAND_UPDATE_RELATION_TREE, CMainFrame::OnUpdateFriendsTree)
	EVT_COMMAND(ID_FriendsTree, EVT_COMMAND_UPDATE_FRIEND_GROUP_ROOT, CMainFrame::OnUpdateFriendGroupRoots)
	EVT_COMMAND(ID_MainFrame, EVT_COMMAND_CLOSE_FRIEND_CHATFRAME, CMainFrame::OnCloseFriendChatFrame)
	EVT_COMMAND(ID_MainFrame, EVT_COMMAND_PROMOTE_FILE_SEND, CMainFrame::OnRecvReqFile)
	EVT_COMMAND(ID_MainFrame, EVT_COMMAND_SHOW_PERCENT, CMainFrame::OnShowPercent)

	EVT_COMMAND(ID_GroupsTree, EVT_COMMAND_INIT_GROUP_TREE, CMainFrame::OnInitGroupsTree)
	EVT_COMMAND(ID_GroupsTree, EVT_COMMAND_UPDATE_GROUP_TREE, CMainFrame::OnUpdateGroupsTree)
	EVT_COMMAND(ID_MainFrame, EVT_COMMAND_UPDATE_GROUP_MEMBER, CMainFrame::OnUpdateGroupMember)
	EVT_COMMAND(ID_MainFrame, EVT_COMMAND_CLOSE_GROUP_CHATFRAME, CMainFrame::OnCloseGroupChatFrame)

	EVT_TREE_ITEM_MENU(ID_FriendsTree, CMainFrame::OnFriendItemMenu)
	EVT_MENU(ID_MenuSelect_Chat_Single, CMainFrame::OnSendSingleChatMsg)
	EVT_MENU(ID_MenuSelect_TransferFile, CMainFrame::OnSendFile)
	EVT_MENU(ID_MenuSelect_DelFriend, CMainFrame::OnDelFriend)
	EVT_MENU(ID_MenuSelect_CreateCustomFriendGroup, CMainFrame::OnCreateFriendGroup)
	EVT_MENU(ID_MenuSelect_DeleteCustomFriendGroup, CMainFrame::OnDeleteFriendGroup)
	EVT_COMMAND(ID_FriendsTree, EVT_COMMAND_RECV_SINGLE_CHATMSG, CMainFrame::OnRecvSingleChatMsg)

	EVT_TREE_ITEM_MENU(ID_GroupsTree, CMainFrame::OnGroupItemMenu)
	EVT_MENU(ID_MenuSelect_Chat_Group, CMainFrame::OnSendGroupChatMsg)
	EVT_MENU(ID_MenuSelect_DelGroup, CMainFrame::OnDelGroup)
	EVT_COMMAND(ID_GroupsTree, EVT_COMMAND_RECV_GROUP_CHATMSG, CMainFrame::OnRecvGroupChatMsg)

	EVT_BUTTON(ID_Menu_Button, CMainFrame::OnClickMenuButton)
	EVT_MENU(ID_Menu_Button_AddFriend, CMainFrame::OnAddFriend)
	EVT_MENU(ID_Menu_Button_AddGroup, CMainFrame::OnAddGroup)
	EVT_MENU(ID_Menu_Button_CreateGroup, CMainFrame::OnCreateGroup)

	EVT_TREE_BEGIN_DRAG(ID_FriendsTree, CMainFrame::OnBeginDrageFriend)
	EVT_TREE_END_DRAG(ID_FriendsTree, CMainFrame::OnEndDrageFriend)

	EVT_CLOSE(CMainFrame::OnClose)
END_EVENT_TABLE()

CMainFrame::CMainFrame()
	: wxFrame( &(TSingleton<CLoginFrame>::GetInstance()), ID_MainFrame, _T(""), wxDefaultPosition, wxSize(300, 500), (wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
{
	_InitUI();
}

void CMainFrame::_InitUI()
{
	wxColour col;
	col.Set(_T("light blue"));
	wxPanel* framePanel = new wxPanel(this, wxID_ANY);
	framePanel->SetBackgroundColour(col);

	wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );

	wxNotebook* noteBook = new wxNotebook(framePanel, wxID_ANY);

	//friend page
	wxPanel* friendsPagePanel = new wxPanel(noteBook, wxID_ANY);
	m_friendsTreeCtrl = new wxTreeCtrl(friendsPagePanel, ID_FriendsTree, wxDefaultPosition, wxSize(300, 400), wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
	m_hiddenRoot = m_friendsTreeCtrl->AddRoot(_T("hidden root"));
	wxTreeItemId friendRootID = m_friendsTreeCtrl->AppendItem(m_hiddenRoot, _T("我的好友"));
	m_mapTreeRoots.insert(std::make_pair(0, friendRootID));

	wxBoxSizer* friendsPageSizer = new wxBoxSizer( wxVERTICAL );
	friendsPageSizer->Add(m_friendsTreeCtrl);
	friendsPagePanel->SetSizer(friendsPageSizer);

	//group page
	wxPanel* groupsPagePanel = new wxPanel(noteBook, wxID_ANY);
	m_groupsTreeCtrl = new wxTreeCtrl(groupsPagePanel, ID_GroupsTree, wxDefaultPosition, wxSize(300, 400));
	m_groupsRootID = m_groupsTreeCtrl->AddRoot(_T("我的群"));

	wxBoxSizer* groupsPageSizer = new wxBoxSizer( wxVERTICAL );
	groupsPageSizer->Add(m_groupsTreeCtrl);
	groupsPagePanel->SetSizer(groupsPageSizer);

	noteBook->AddPage(friendsPagePanel, _T("好友"));
	noteBook->AddPage(groupsPagePanel, _T("群"));

	wxButton* menuBtn = new wxButton(framePanel, ID_Menu_Button, _T("菜单"));

	topsizer->Add(noteBook);
	topsizer->Add(menuBtn);

	framePanel->SetSizer(topsizer);
}

void CMainFrame::OnInitFriendsTree( wxCommandEvent& event )
{
	SRelation& relation = TSingleton<CClientPlayer>::GetInstance().GetRelation();
	std::map<uint32, SFriendGroupData> mapFriendGroups = relation.m_mapFriendGroups;
	std::map<uint32, SFriendGroupData>::iterator friendGroupIter = mapFriendGroups.begin();
	for (; friendGroupIter != mapFriendGroups.end(); ++friendGroupIter)
	{
		if (friendGroupIter->first != 0)
		{
			wxTreeItemId rootID = m_friendsTreeCtrl->AppendItem(m_hiddenRoot, friendGroupIter->second.m_strFriendGroupName.c_str());
			if (rootID.IsOk())
			{
				m_mapTreeRoots.insert(std::make_pair(friendGroupIter->first, rootID));
			}
		}
	}

	std::map<uint32, SRelationData> mapRelation = relation.m_mapRelations;
	std::map<uint32, SRelationData>::iterator friendIter = mapRelation.begin();
	for (; friendIter != mapRelation.end(); ++friendIter)
	{
		XString strState = friendIter->second.m_bIsOnline ? _T("在线") : _T("离线");
		XString strNickName = friendIter->second.m_strNickname + _T("(") + strState + _T(")");
		wxTreeItemId rootID = _GetTreeRootByFriendGroupID(friendIter->second.m_unFriendGroupID);
		if (rootID.IsOk())
		{
			wxTreeItemId curID = m_friendsTreeCtrl->AppendItem(rootID, strNickName.c_str(), -1, -1, NULL);
			if (curID.IsOk())
			{
				m_mapFriends.insert(std::make_pair(friendIter->second.m_unCharactorID, curID));
			}
		}
	}
}

void CMainFrame::OnUpdateFriendsTree( wxCommandEvent& event )
{
	uint32 updateID = event.GetInt();
	SRelation& relation = TSingleton<CClientPlayer>::GetInstance().GetRelation();
	std::map<uint32, wxTreeItemId>::iterator idIter = m_mapFriends.find(updateID);
	if (idIter != m_mapFriends.end())
	{
		SRelationData* pRelationData = relation.FindRelation(updateID);
		if (pRelationData != NULL)
		{
			XString strState = pRelationData->m_bIsOnline ? _T("在线") : _T("离线");
			XString strNickName = pRelationData->m_strNickname + _T("(") + strState + _T(")");

			//Update friend group
			wxTreeItemId newRootID = _GetTreeRootByFriendGroupID(pRelationData->m_unFriendGroupID);
			wxTreeItemId oldRootID = m_friendsTreeCtrl->GetItemParent(idIter->second);
			if (newRootID.IsOk() && oldRootID.IsOk() && newRootID != oldRootID)
			{
				//remove from the old root
				m_friendsTreeCtrl->Delete(idIter->second);
				m_mapFriends.erase(idIter->first);

				//add to new root
				wxTreeItemId curID = m_friendsTreeCtrl->AppendItem(newRootID, strNickName.c_str(), -1, -1, NULL);
				if (curID.IsOk())
				{
					m_mapFriends.insert(std::make_pair(pRelationData->m_unCharactorID, curID));
				}
			}
			else
			{
				//Update friend info
				m_friendsTreeCtrl->SetItemText(idIter->second, strNickName.c_str());
			}
		}
		else
		{
			//delete friend
			m_friendsTreeCtrl->Delete(idIter->second);
			m_mapFriendChatFrames.erase(idIter->first);
			m_mapFriends.erase(idIter->first);
		}
	}
	else
	{
		//add friend
		SRelationData* pRelationData = relation.FindRelation(updateID);
		if (pRelationData != NULL)
		{
			XString strState = pRelationData->m_bIsOnline ? _T("在线") : _T("离线");
			XString strNickName = pRelationData->m_strNickname + _T("(") + strState + _T(")");
			wxTreeItemId rootID = _GetTreeRootByFriendGroupID(pRelationData->m_unCharactorID);
			if (rootID.IsOk())
			{
				wxTreeItemId curID = m_friendsTreeCtrl->AppendItem(rootID, strNickName.c_str(), -1, -1, NULL);
				if (curID.IsOk())
				{
					m_mapFriends.insert(std::make_pair(pRelationData->m_unCharactorID, curID));
				}
			}
		}
	}
}

void CMainFrame::OnUpdateFriendGroupRoots( wxCommandEvent& event )
{
	uint32 updateID = event.GetInt();
	SRelation& relation = TSingleton<CClientPlayer>::GetInstance().GetRelation();
	std::map<uint32, wxTreeItemId>::iterator idIter = m_mapTreeRoots.find(updateID);
	if (idIter != m_mapTreeRoots.end())
	{
		SFriendGroupData* pFriendGroup = relation.FindFriendGroup(updateID);
		if (pFriendGroup != NULL)
		{
			m_friendsTreeCtrl->SetItemText(idIter->second, pFriendGroup->m_strFriendGroupName.c_str());
		}
		else
		{
			//delete friend group
			m_friendsTreeCtrl->Delete(idIter->second);
			m_mapTreeRoots.erase(updateID);
		}
	}
	else
	{
		//add friend group
		SFriendGroupData* pFriendGroup = relation.FindFriendGroup(updateID);
		if (pFriendGroup != NULL)
		{
			wxTreeItemId rootID = m_friendsTreeCtrl->AppendItem(m_hiddenRoot, pFriendGroup->m_strFriendGroupName.c_str());
			if (rootID.IsOk())
			{
				m_mapTreeRoots.insert(std::make_pair(pFriendGroup->m_unFriendGroupID, rootID));
			}
		}
	}
}

void CMainFrame::OnFriendItemMenu( wxTreeEvent& event )
{
	uint32 unFriendGroupID = _GetFriendGroupIDByTreeRoot(event.GetItem());
	if (unFriendGroupID != INVALID_FRIEND_GROUP_ID)
	{
		m_curSelectedRootItemID = event.GetItem();

		wxMenu menu;
		menu.Append(ID_MenuSelect_CreateCustomFriendGroup, _T("添加分组"));
		if (unFriendGroupID != 0)
		{
			menu.Append(ID_MenuSelect_DeleteCustomFriendGroup, _T("删除分组"));
		}

		PopupMenu(&menu, wxDefaultPosition);
	}
	else
	{
		m_curSelectedFriendItemID = event.GetItem();

		wxMenu menu;
		menu.Append(ID_MenuSelect_Chat_Single, _T("发送即时消息"));
		menu.Append(ID_MenuSelect_TransferFile, _T("发送文件"));
		menu.Append(ID_MenuSelect_DelFriend, _T("删除好友"));

		PopupMenu(&menu, wxDefaultPosition);
	}
}

uint32 CMainFrame::_GetPlayerIDByItemID( wxTreeItemId treeItemID )
{
	std::map<uint32, wxTreeItemId>::iterator iter = m_mapFriends.begin();
	for (; iter != m_mapFriends.end(); ++iter)
	{
		if (iter->second == treeItemID)
		{
			return iter->first;
		}
	}

	return INVALID_PLAYER_ID;
}

uint32 CMainFrame::_GetGroupIDByItemID( wxTreeItemId treeItemID )
{
	std::map<uint32, wxTreeItemId>::iterator iter = m_mapGroupItems.begin();
	for (; iter != m_mapGroupItems.end(); ++iter)
	{
		if (iter->second == treeItemID)
		{
			return iter->first;
		}
	}

	return INVALID_GROUP_ID;
}

wxTreeItemId CMainFrame::_GetTreeRootByFriendGroupID( uint32 friendGroupID )
{
	std::map<uint32, wxTreeItemId>::iterator iter = m_mapTreeRoots.find(friendGroupID);
	if (iter != m_mapTreeRoots.end())
	{
		return iter->second;
	}
	
	return wxTreeItemId();
}

uint32 CMainFrame::_GetFriendGroupIDByTreeRoot( wxTreeItemId treeItemID )
{
	std::map<uint32, wxTreeItemId>::iterator iter = m_mapTreeRoots.begin();
	for (; iter != m_mapTreeRoots.end(); ++iter)
	{
		if (iter->second == treeItemID)
		{
			return iter->first;
		}
	}

	return INVALID_FRIEND_GROUP_ID;
}

void CMainFrame::OnSendSingleChatMsg( wxCommandEvent& event )
{
	if (m_curSelectedFriendItemID.IsOk())
	{
		uint32 targetID = _GetPlayerIDByItemID(m_curSelectedFriendItemID);
		std::map<uint32, CFriendChatFrame*>::iterator frameIter = m_mapFriendChatFrames.find(targetID);
		if (frameIter != m_mapFriendChatFrames.end())
		{
			if (frameIter->second != NULL)
			{
				frameIter->second->SetFocus();
			}
		}
		else
		{
			CFriendChatFrame* pChatFrame = new CFriendChatFrame(targetID, TSingleton<CClientPlayer>::GetInstance().GetFriendName(targetID), this);
			pChatFrame->Show();
			m_mapFriendChatFrames.insert(std::make_pair(targetID, pChatFrame));
		}
	}
}

void CMainFrame::OnRecvSingleChatMsg( wxCommandEvent& event )
{
	uint32 senderID = event.GetInt();
	wxString strContent = event.GetString();
	time_t nowTime = event.GetTimestamp();
	std::map<uint32, CFriendChatFrame*>::iterator frameIter = m_mapFriendChatFrames.find(senderID);
	if (frameIter != m_mapFriendChatFrames.end())
	{
		if (frameIter->second != NULL)
		{
			frameIter->second->OnRecv(strContent.c_str(), nowTime);
		}
	}
	else
	{
		CFriendChatFrame* pChatFrame = new CFriendChatFrame(senderID, TSingleton<CClientPlayer>::GetInstance().GetFriendName(senderID), this);
		pChatFrame->Show();
		pChatFrame->OnRecv(strContent.c_str(), nowTime);
		m_mapFriendChatFrames.insert(std::make_pair(senderID, pChatFrame));
	}
}

void CMainFrame::OnCloseFriendChatFrame(wxCommandEvent& event)
{
	m_mapFriendChatFrames.erase(event.GetInt());
}

void CMainFrame::OnClickMenuButton( wxCommandEvent& event )
{
	wxMenu menu;
	menu.Append(ID_Menu_Button_AddFriend, _T("添加好友"));
	menu.Append(ID_Menu_Button_AddGroup, _T("添加群"));
	menu.Append(ID_Menu_Button_CreateGroup, _T("创建群"));
	PopupMenu(&menu, wxDefaultPosition);
}

void CMainFrame::OnAddFriend( wxCommandEvent& event )
{
	CFriendAddFrame* pFrame = new CFriendAddFrame;
	pFrame->Show();
}

void CMainFrame::OnDelFriend( wxCommandEvent& event )
{
	uint32 playerID = _GetPlayerIDByItemID(m_curSelectedFriendItemID);
	SRelation& relation = TSingleton<CClientPlayer>::GetInstance().GetRelation();
	SRelationData* pRelationData = relation.FindRelation(playerID);
	if (pRelationData != NULL)
	{
		TSingleton<CClientGUI>::GetInstance().OnDelFriend(pRelationData->m_unCharactorID);
	}
}

void CMainFrame::OnInitGroupsTree( wxCommandEvent& event )
{
	SGroups& rGroups = TSingleton<CClientPlayer>::GetInstance().GetGroups();
	SGroups::MAPGROUPS::iterator groupIter = rGroups.m_mapGroups.begin();
	for (; groupIter != rGroups.m_mapGroups.end(); ++groupIter)
	{
		wxTreeItemId curID = m_groupsTreeCtrl->AppendItem(m_groupsRootID, groupIter->second.m_strGroupName.c_str(), -1, -1, NULL);
		m_mapGroupItems.insert(std::make_pair(groupIter->second.m_unGroupID, curID));
	}
}

void CMainFrame::OnUpdateGroupsTree( wxCommandEvent& event )
{
	uint32 updateID = event.GetInt();
	SGroups& rGroups = TSingleton<CClientPlayer>::GetInstance().GetGroups();
	std::map<uint32, wxTreeItemId>::iterator idIter = m_mapGroupItems.find(updateID);
	if (idIter != m_mapGroupItems.end())
	{
		SGroupData* pGroupData = rGroups.GetGroup(updateID);
		if (pGroupData != NULL)
		{
			m_groupsTreeCtrl->SetItemText(idIter->second, pGroupData->m_strGroupName.c_str());
		}
		else
		{
			//delete group
			m_groupsTreeCtrl->Delete(idIter->second);
			m_mapGroupChatFrames.erase(idIter->first);
			m_mapGroupItems.erase(idIter->first);
		}
	}
	else
	{
		//add group
		SGroupData* pGroupData = rGroups.GetGroup(updateID);
		if (pGroupData != NULL)
		{
			wxTreeItemId curID = m_groupsTreeCtrl->AppendItem(m_groupsRootID, pGroupData->m_strGroupName.c_str(), -1, -1, NULL);
			m_mapGroupItems.insert(std::make_pair(pGroupData->m_unGroupID, curID));
		}
	}
}

void CMainFrame::OnUpdateGroupMember( wxCommandEvent& event )
{
	uint32 groupID = event.GetInt();
	uint32 memberID = static_cast<uint32>(event.GetExtraLong());

	std::map<uint32, CGroupChatFrame*> ::iterator frameIter = m_mapGroupChatFrames.find(groupID);
	if (frameIter != m_mapGroupChatFrames.end())
	{
		frameIter->second->UpdateMemberList(memberID);
	}
}

void CMainFrame::OnGroupItemMenu( wxTreeEvent& event )
{
	if (m_groupsRootID == event.GetItem())
	{
		return;
	}

	m_curSelectedGroupItemID = event.GetItem();

	wxMenu menu;
	menu.Append(ID_MenuSelect_Chat_Group, _T("发送群消息"));
	menu.Append(ID_MenuSelect_DelGroup, _T("退出群"));

	PopupMenu(&menu, wxDefaultPosition);
}

void CMainFrame::OnAddGroup( wxCommandEvent& event )
{
	CGroupAddFrame* pFrame = new CGroupAddFrame;
	pFrame->Show();
}

void CMainFrame::OnCreateGroup( wxCommandEvent& event )
{
	CGroupCreateFrame* pFrame = new CGroupCreateFrame;
	pFrame->Show();
}

void CMainFrame::OnDelGroup( wxCommandEvent& event )
{
	uint32 groupID = _GetGroupIDByItemID(m_curSelectedGroupItemID);
	SGroups& rGroups = TSingleton<CClientPlayer>::GetInstance().GetGroups();
	SGroupData* pGroup = rGroups.GetGroup(groupID);
	if (pGroup != NULL)
	{
		TSingleton<CClientGUI>::GetInstance().OnDelGroup(pGroup->m_unGroupID);
	}
}

void CMainFrame::OnCloseGroupChatFrame( wxCommandEvent& event )
{
	m_mapGroupChatFrames.erase(event.GetInt());
}

void CMainFrame::OnSendGroupChatMsg( wxCommandEvent& event )
{
	if (m_curSelectedGroupItemID.IsOk())
	{
		uint32 targetID = _GetGroupIDByItemID(m_curSelectedGroupItemID);
		std::map<uint32, CGroupChatFrame*>::iterator frameIter = m_mapGroupChatFrames.find(targetID);
		if (frameIter != m_mapGroupChatFrames.end())
		{
			if (frameIter->second != NULL)
			{
				frameIter->second->SetFocus();
			}
		}
		else
		{
			CGroupChatFrame* pChatFrame = new CGroupChatFrame(targetID, TSingleton<CClientPlayer>::GetInstance().GetGroupName(targetID), this);
			pChatFrame->Show();
			m_mapGroupChatFrames.insert(std::make_pair(targetID, pChatFrame));
		}
	}
}

void CMainFrame::OnRecvGroupChatMsg( wxCommandEvent& event )
{
	uint32 senderID = event.GetInt();
	uint32 groupID = static_cast<uint32>(event.GetExtraLong());
	wxString strContent = event.GetString();
	time_t nowTime = event.GetTimestamp();
	std::map<uint32, CGroupChatFrame*>::iterator frameIter = m_mapGroupChatFrames.find(groupID);
	if (frameIter != m_mapGroupChatFrames.end())
	{
		if (frameIter->second != NULL)
		{
			frameIter->second->OnRecv(senderID, strContent.c_str(), nowTime);
		}
	}
	else
	{
		CGroupChatFrame* pChatFrame = new CGroupChatFrame(groupID, TSingleton<CClientPlayer>::GetInstance().GetGroupName(groupID), this);
		pChatFrame->Show();
		pChatFrame->OnRecv(senderID, strContent.c_str(), nowTime);
		m_mapGroupChatFrames.insert(std::make_pair(groupID, pChatFrame));
	}
}

void CMainFrame::OnClose( wxCloseEvent& event )
{
	TSingleton<CClientGUI>::GetInstance().OnReqExitWorld();
	TSingleton<CClientGUI>::GetInstance().Shutdown();
	TSingleton<CLoginFrame>::GetInstance().Destroy();
	Destroy();
}

void CMainFrame::OnSendFile( wxCommandEvent& event )
{
	wxString wildCard = _T("ALL files (*.*)|*.*|TXT files (*.txt)|*.txt");
	wxString defaultDir = _T("f:\\");
	wxString defaultFileName = wxEmptyString;
	wxFileDialog dialog(NULL, _T("选择文件"), defaultDir, defaultFileName, wildCard, wxOPEN);
	if (dialog.ShowModal() == wxID_OK)
	{
		if (m_curSelectedFriendItemID.IsOk())
		{
			uint32 targetID = _GetPlayerIDByItemID(m_curSelectedFriendItemID);
			wxString path = dialog.GetPath();
			TSingleton<CFileTransManager>::GetInstance().SendFileReq(targetID, path.c_str());
		}
	}
}

void CMainFrame::OnRecvReqFile( wxCommandEvent& event )
{
	MsgReqSendFile* pData = static_cast<MsgReqSendFile*>(event.GetClientData());
	uint32 unSenderID = pData->m_unSenderID;
	uint32 unLocalFileNO = pData->m_unLocalFileNO;
	XString strSenderName = pData->m_strSenderName;
	XString strFileName = pData->m_strFileName;
	uint32 fileLength = pData->m_unFileTotalLength;
	XString strContent = XString().Format(_T("%s请求发送文件:%s(%dBytes)"), strSenderName.c_str(), strFileName.c_str(), fileLength);
	int answer = wxMessageBox(strContent.c_str(), wxMessageBoxCaptionStr, wxYES_NO);
	if (answer == wxYES)
	{
		wxString defaultDir = _T("f:\\");
		wxDirDialog dialog(NULL, _T("选择目录"), defaultDir, wxDD_NEW_DIR_BUTTON);
		if (dialog.ShowModal() == wxID_OK)
		{
			wxString path = dialog.GetPath();
			path += strFileName.c_str();
			TSingleton<CFileTransManager>::GetInstance().SendFileAck(unSenderID, unLocalFileNO, path.c_str(), fileLength);
			return;
		}
	}

	TSingleton<CFileTransManager>::GetInstance().SendFileAck(unSenderID, unLocalFileNO, _T(""), fileLength);
}

void CMainFrame::OnShowPercent( wxCommandEvent& event )
{
	SFile* pFile = static_cast<SFile*>(event.GetClientData());
	uint32 key = pFile->m_unLocalFileNO;
	XString strFileName = pFile->m_strFilePath;
	uint32 totalValue = pFile->m_untotalLen;
	uint32 currentValue = pFile->m_bIsSender ? pFile->m_unTotalSent: pFile->m_unTotalReceived;
	uint32 percent = (totalValue != 0) ? static_cast<double>(currentValue)/totalValue*100 : 0;

	std::map<uint32, CFileGuageFrame*>::iterator guageIter = m_mapFileGuages.find(key);
	if (guageIter == m_mapFileGuages.end())
	{
		CFileGuageFrame* pFileGuageFrame = new CFileGuageFrame(strFileName.c_str());
		pFileGuageFrame->Show();
		pFileGuageFrame->OnUpdateGuage(percent);
		m_mapFileGuages.insert(std::make_pair(key, pFileGuageFrame));
	}
	else
	{
		guageIter->second->OnUpdateGuage(percent);
		if (percent >= 100)
		{
			m_mapFileGuages.erase(key);
			wxMessageBox(_T("Complete"));
		}
	}
}

void CMainFrame::OnCreateFriendGroup( wxCommandEvent& event )
{
	CFriendGroupCreateFrame* pFrame = new CFriendGroupCreateFrame;
	pFrame->Show();
}

void CMainFrame::OnDeleteFriendGroup( wxCommandEvent& event )
{
	if (m_curSelectedRootItemID.IsOk())
	{
		if (m_friendsTreeCtrl->GetChildrenCount(m_curSelectedRootItemID) > 0)
		{
			wxMessageBox(_T("无法删除非空的分组"));
		}
		else
		{
			uint32 unFriendGroupID = _GetFriendGroupIDByTreeRoot(m_curSelectedRootItemID);
			TSingleton<CClientGUI>::GetInstance().OnReqDeleteFriendGroup(unFriendGroupID);
		}
	}
}

void CMainFrame::OnBeginDrageFriend(wxTreeEvent& event)
{
	if (_GetFriendGroupIDByTreeRoot(event.GetItem()) != INVALID_FRIEND_GROUP_ID)
	{
		return;
	}
	m_lastDragItem = event.GetItem();
	event.Allow();
}

void CMainFrame::OnEndDrageFriend( wxTreeEvent& event )
{
	wxTreeItemId curNode = event.GetItem();
	if (m_lastDragItem.IsOk() && curNode.IsOk() && m_lastDragItem != curNode)
	{
		uint32 playerID = _GetPlayerIDByItemID(m_lastDragItem);
		uint32 friendGroupID = _GetFriendGroupIDByTreeRoot(curNode);
		TSingleton<CClientGUI>::GetInstance().OnReqChangeFriendGroup(playerID, friendGroupID);

		m_lastDragItem.Unset();
	}
}