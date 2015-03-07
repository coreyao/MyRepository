#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_MAINFRAME_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_MAINFRAME_H__INCLUDED

#include <map>

#include "UI/UICommon.h"
#include "wx/treectrl.h"
#include "wx/notebook.h"

#include "XString/XString.h"

using Core::Utility::uint32;
using Core::Utility::uint64;

class MyTreeItemData : public wxTreeItemData
{
public:
	MyTreeItemData(uint32 playerID) : m_unPlayerID(0) { }
	uint32 GetPlayerID() {return m_unPlayerID;}
private :
	uint32 m_unPlayerID;
};

class CFriendChatFrame;
class CGroupChatFrame;
class CFileGuageFrame;
class CMainFrame : public wxFrame
{
public:
	CMainFrame();
	void OnInitFriendsTree(wxCommandEvent& event);
	void OnUpdateFriendsTree(wxCommandEvent& event);
	void OnUpdateFriendGroupRoots(wxCommandEvent& event);
	void OnFriendItemMenu(wxTreeEvent& event);
	void OnSendSingleChatMsg(wxCommandEvent& event);
	void OnRecvSingleChatMsg(wxCommandEvent& event);
	void OnCloseFriendChatFrame(wxCommandEvent& event);
	void OnAddFriend(wxCommandEvent& event);
	void OnDelFriend(wxCommandEvent& event);
	void OnSendFile(wxCommandEvent& event);
	void OnRecvReqFile(wxCommandEvent& event);
	void OnShowPercent(wxCommandEvent& event);
	void OnCreateFriendGroup(wxCommandEvent& event);
	void OnDeleteFriendGroup(wxCommandEvent& event);

	void OnInitGroupsTree(wxCommandEvent& event);
	void OnUpdateGroupsTree(wxCommandEvent& event);
	void OnUpdateGroupMember(wxCommandEvent& event);
	void OnGroupItemMenu(wxTreeEvent& event);
	void OnSendGroupChatMsg(wxCommandEvent& event);
	void OnRecvGroupChatMsg(wxCommandEvent& event);
	void OnCloseGroupChatFrame(wxCommandEvent& event);
	void OnAddGroup(wxCommandEvent& event);
	void OnDelGroup(wxCommandEvent& event);
	void OnCreateGroup(wxCommandEvent& event);

	void OnClickMenuButton(wxCommandEvent& event);
	void OnBeginDrageFriend(wxTreeEvent& event);
	void OnEndDrageFriend(wxTreeEvent& event);

	void OnClose(wxCloseEvent& event);

private:
	void _InitUI();
	uint32 _GetPlayerIDByItemID(wxTreeItemId treeItemID);
	uint32 _GetGroupIDByItemID(wxTreeItemId treeItemID);
	wxTreeItemId _GetTreeRootByFriendGroupID(uint32 friendGroupID);
	uint32 _GetFriendGroupIDByTreeRoot(wxTreeItemId treeItemID);

private:
	wxTreeCtrl* m_friendsTreeCtrl;
	std::map<uint32, wxTreeItemId> m_mapTreeRoots;
	wxTreeItemId m_hiddenRoot;
	wxTreeItemId m_lastDragItem;
	wxTreeItemId m_curSelectedRootItemID;

	wxTreeCtrl* m_groupsTreeCtrl;
	wxTreeItemId m_groupsRootID;

	wxTreeItemId m_curSelectedFriendItemID;
	std::map<uint32, wxTreeItemId> m_mapFriends;
	std::map<uint32, CFriendChatFrame*> m_mapFriendChatFrames;

	wxTreeItemId m_curSelectedGroupItemID;
	std::map<uint32, wxTreeItemId> m_mapGroupItems;
	std::map<uint32, CGroupChatFrame*> m_mapGroupChatFrames;
	
	std::map<uint32, CFileGuageFrame*> m_mapFileGuages;

	DECLARE_EVENT_TABLE()
};

#endif