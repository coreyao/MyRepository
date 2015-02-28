#include "UI/FriendChatFrame.h"
#include "ClientGUI/ClientGUI.h"
#include "ClientGUI/ClientPlayer.h"
#include "Singleton.h"
#include "UI/MainFrame.h"
#include "TimeInfo/TimeInfo.h"
#include "UI/ChatHistoryFrame.h"

using Core::Utility::TSingleton;
using Core::Utility::CTimeInfo;

BEGIN_EVENT_TABLE(CFriendChatFrame, wxFrame)
	EVT_BUTTON(ID_SendFriendText_Button, CFriendChatFrame::OnSend)
	EVT_BUTTON(ID_ShowSingleChatHistory_Button, CFriendChatFrame::OnShowChatHistory)
	EVT_CLOSE(CFriendChatFrame::OnClose)
END_EVENT_TABLE()

CFriendChatFrame::CFriendChatFrame(uint32 unFriendID, const XString& strTitle, wxWindow* parent)
	: wxFrame(parent, wxID_ANY, strTitle.c_str(), wxDefaultPosition, wxSize(500,500), (wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
	, m_unPlayerID(unFriendID)
{
	_InitUI();
}

void CFriendChatFrame::_InitUI()
{
	m_panel = new wxPanel(this, wxID_ANY);
	m_output_TextCtrl = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxPoint(10,10), wxSize(470,300),wxTE_MULTILINE | wxTE_READONLY);
	m_input_TextCtrl = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxPoint(10,330), wxSize(470,70));
	m_input_TextCtrl->SetFocus();
	m_send_button = new wxButton(m_panel, ID_SendFriendText_Button, _T("发送"), wxPoint(10,420));
	m_history_button = new wxButton(m_panel, ID_ShowSingleChatHistory_Button, _T("查看历史"), wxPoint(100, 420));
}

void CFriendChatFrame::OnSend( wxCommandEvent& event )
{
	wxString strContent = m_input_TextCtrl->GetValue();
	if (strContent.IsEmpty())
	{
		wxMessageBox(_T("不能发送空白消息"));
		return;
	}

	time_t nowTime = ::time(NULL);
	XString strMsg = TSingleton<CClientPlayer>::GetInstance().GetNickName();
	strMsg += _T(" ");
	strMsg += CTimeInfo(nowTime).ToString();
	strMsg += _T("\n");
	strMsg += strContent.c_str();
	strMsg += _T("\n");

	m_output_TextCtrl->AppendText(strMsg);

	TSingleton<CClientGUI>::GetInstance().OnSendSingleChat(m_unPlayerID, strContent.c_str(), nowTime);

	m_input_TextCtrl->Clear();
	m_input_TextCtrl->SetFocus();
}

void CFriendChatFrame::OnRecv(const XString& strContent, time_t nowTime)
{
	if (strContent.empty())
	{
		return;
	}

	XString strMsg = TSingleton<CClientPlayer>::GetInstance().GetFriendName(m_unPlayerID);
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

void CFriendChatFrame::OnClose( wxCloseEvent& event )
{
	wxCommandEvent evt(EVT_COMMAND_CLOSE_FRIEND_CHATFRAME, ID_MainFrame);
	evt.SetInt(m_unPlayerID);
	wxPostEvent(&(TSingleton<CMainFrame>::GetInstance()), evt);

	Destroy();
}

void CFriendChatFrame::OnShowChatHistory( wxCommandEvent& event )
{
	XString strMsg = TSingleton<CClientPlayer>::GetInstance().GetFriendName(m_unPlayerID);
	CChatHistoryFrame* pFrame = new CChatHistoryFrame(strMsg.c_str(), this);
	pFrame->OnShowSingleChatHistory(m_unPlayerID);
	pFrame->Show();
}
