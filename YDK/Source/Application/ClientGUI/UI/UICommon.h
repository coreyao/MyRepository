#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_IDLIST_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_IDLIST_H__INCLUDED

#include <wx/wx.h>

enum
{
	ID_LoginButton,
	ID_LoginFrame,
	ID_RegisterButton,
	ID_MainFrame,
	ID_FriendsTree,
	ID_GroupsTree,
	ID_MenuSelect_Chat_Single,
	ID_MenuSelect_Chat_Group,
	ID_MenuSelect_TransferFile,
	ID_MenuSelect_DelFriend,
	ID_MenuSelect_DelGroup,
	ID_MenuSelect_CreateCustomFriendGroup,
	ID_MenuSelect_DeleteCustomFriendGroup,
	ID_SendFriendText_Button,
	ID_ShowSingleChatHistory_Button,
	ID_ShowGroupChatHistory_Button,
	ID_SendGroupText_Button,
	ID_Menu_Button,
	ID_Menu_Button_AddFriend,
	ID_Menu_Button_AddGroup,
	ID_Menu_Button_CreateGroup,
	ID_Button_Confirm_AddFriend,
	ID_Button_Confirm_AddGroup,
	ID_Button_Confirm_CreateGroup,
	ID_Button_Confirm_CreateFriendGroup,
	ID_ListBox_Group,
	ID_Register_Confirm,
	ID_Register_Reset,
	ID_Register_Frame,
};

DECLARE_EVENT_TYPE(EVT_COMMAND_CREATE_MAIN_FRAME, -1)

DECLARE_EVENT_TYPE(EVT_COMMAND_INIT_RELATION_TREE, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_UPDATE_RELATION_TREE, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_UPDATE_FRIEND_GROUP_ROOT, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_RECV_SINGLE_CHATMSG, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_CLOSE_FRIEND_CHATFRAME, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_PROMOTE_FILE_SEND, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_SHOW_PERCENT, -1)

DECLARE_EVENT_TYPE(EVT_COMMAND_INIT_GROUP_TREE, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_UPDATE_GROUP_TREE, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_UPDATE_GROUP_MEMBER, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_RECV_GROUP_CHATMSG, -1)
DECLARE_EVENT_TYPE(EVT_COMMAND_CLOSE_GROUP_CHATFRAME, -1)

DECLARE_EVENT_TYPE(EVT_COMMAND_RECV_REGISTER_RESULT, -1)

#endif