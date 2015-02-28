#include "UI/GroupChatFrame.h"
#include "ClientGUI/ClientGUI.h"
#include "ClientGUI/ClientPlayer.h"
#include "Singleton.h"
#include "UI/MainFrame.h"
#include "TimeInfo/TimeInfo.h"
#include "UI/ChatHistoryFrame.h"

using Core::Utility::CTimeInfo;

BEGIN_EVENT_TABLE(CGroupChatFrame, wxFrame)
	EVT_BUTTON(ID_SendGroupText_Button, CGroupChatFrame::OnSend)
	EVT_CLOSE(CGroupChatFrame::OnClose)
	EVT_BUTTON(ID_ShowGroupChatHistory_Button, CGroupChatFrame::OnShowChatHistory)
END_EVENT_TABLE()

CGroupChatFrame::CGroupChatFrame(uint32 unGroupID, const XString& strTitle, wxWindow* parent)
	: wxFrame(parent, wxID_ANY, strTitle.c_str(), wxDefaultPosition, wxSize(500,500), (wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
	, m_unGroupID(unGroupID)
{
	_InitUI();
	_InitMemberList();
}

void CGroupChatFrame::OnSend( wxCommandEvent& event )
{
	wxString strContent = m_input_TextCtrl->GetValue();
	if (strContent.IsEmpty())
	{
		wxMessageBox(_T("不能发送空白消息"));
		return;
	}

	TSingleton<CClientGUI>::GetInstance().OnSendGroupChat(m_unGroupID, strContent.c_str(), ::time(NULL));

	m_input_TextCtrl->Clear();
	m_input_TextCtrl->SetFocus();
}

void CGroupChatFrame::OnRecv(uint32 unGroupMemberID, const XString& strContent, time_t nowTime )
{
	if (strContent.empty())
	{
		return;
	}

	XString strMsg = TSingleton<CClientPlayer>::GetInstance().GetGroupMemberName(m_unGroupID, unGroupMemberID);
	if (strMsg.empty())
	{
		return;
	}

	strMsg += _T(" ");
	strMsg += CTimeInfo(nowTime).ToString();
	strMsg += _T("\n");
	strMsg += strContent.c_str();
	strMsg += _T("\n");

	m_output_TextCtrl->AppendText(strMsg);
}

void CGroupChatFrame::OnClose( wxCloseEvent& event )
{
	wxCommandEvent evt(EVT_COMMAND_CLOSE_GROUP_CHATFRAME, ID_MainFrame);
	evt.SetInt(m_unGroupID);
	wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);

	Destroy();
}

void CGroupChatFrame::_InitUI()
{
	m_panel = new wxPanel(this, wxID_ANY);
	m_output_TextCtrl = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxPoint(10,10), wxSize(380,300), wxTE_MULTILINE | wxTE_READONLY);
	m_memberList = new wxListBox(m_panel, ID_ListBox_Group, wxPoint(400,10), wxSize(70,390));
	m_input_TextCtrl = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxPoint(10,330), wxSize(380,70));
	m_input_TextCtrl->SetFocus();
	m_send_button = new wxButton(m_panel, ID_SendGroupText_Button, _T("发送"), wxPoint(10, 420));
	m_history_button = new wxButton(m_panel, ID_ShowGroupChatHistory_Button, _T("查看历史"), wxPoint(100, 420));
}

void CGroupChatFrame::_InitMemberList()
{
	SGroupData* pGroupData = TSingleton<CClientPlayer>::GetInstance().GetGroups().GetGroup(m_unGroupID);
	if (pGroupData != NULL)
	{
		SGroupData::MAPGROUPMEMBERS::iterator memberIter = pGroupData->m_mapMembers.begin();
		for (; memberIter != pGroupData->m_mapMembers.end(); ++memberIter)
		{
			XString strState = memberIter->second.m_bIsOnline ? _T("在线") : _T("离线");
			XString strNickName = memberIter->second.m_strName + _T("(") + strState + _T(")");
			int32 curID = m_memberList->Append(strNickName.c_str());
			m_mapMemberItems.insert(std::make_pair(memberIter->second.m_unMemeberID, curID));
		}
	}
}

void CGroupChatFrame::UpdateMemberList( uint32 unMemberID )
{
	SGroupData* pGroupData = TSingleton<CClientPlayer>::GetInstance().GetGroups().GetGroup(m_unGroupID);
	if (pGroupData != NULL)
	{
		std::map<uint32, int32>::iterator itemIter = m_mapMemberItems.find(unMemberID);
		if (itemIter != m_mapMemberItems.end())
		{
			SGroupMemeber* pMember = pGroupData->GetMember(unMemberID);
			if (pMember != NULL)
			{
				XString strState = pMember->m_bIsOnline ? _T("在线") : _T("离线");
				XString strNickName = pMember->m_strName + _T("(") + strState + _T(")");
				m_memberList->SetString(itemIter->second, strNickName.c_str());
			}
			else
			{
				m_memberList->Delete(itemIter->second);
				m_mapMemberItems.erase(itemIter);
			}
		}
		else
		{
			SGroupMemeber* pMember = pGroupData->GetMember(unMemberID);
			if (pMember != NULL)
			{
				XString strState = pMember->m_bIsOnline ? _T("在线") : _T("离线");
				XString strNickName = pMember->m_strName + _T("(") + strState + _T(")");
				int32 curID = m_memberList->Append(strNickName.c_str());
				m_mapMemberItems.insert(std::make_pair(pMember->m_unMemeberID, curID));
			}
		}
	}
}

void CGroupChatFrame::OnShowChatHistory( wxCommandEvent& event )
{
	SGroupData* pGroupData = TSingleton<CClientPlayer>::GetInstance().GetGroups().GetGroup(m_unGroupID);
	if (pGroupData != NULL)
	{
		CChatHistoryFrame* pFrame = new CChatHistoryFrame(pGroupData->m_strGroupName.c_str(), this);
		pFrame->OnShowGroupChatHistory(m_unGroupID);
		pFrame->Show();
	}
}