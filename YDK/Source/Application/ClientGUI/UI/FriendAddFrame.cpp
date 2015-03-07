#include "UI/FriendAddFrame.h"
#include "ClientGUI/ClientGUI.h"

BEGIN_EVENT_TABLE(CFriendAddFrame, wxFrame)
	EVT_BUTTON(ID_Button_Confirm_AddFriend, CFriendAddFrame::OnConfirmFriendName)
END_EVENT_TABLE()

CFriendAddFrame::CFriendAddFrame()
	: wxFrame( NULL, wxID_ANY, _T("添加好友"), wxDefaultPosition, wxSize(250, 200), (wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
{
	_InitUI();
}

void CFriendAddFrame::_InitUI()
{
	wxColour col;
	col.Set("light blue");
	wxPanel* panel = new wxPanel(this,wxID_ANY);
	panel->SetBackgroundColour(col);

	new wxStaticText(panel, wxID_ANY, _T("好友账号： "), wxPoint(30,30));
	m_input_TextCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxPoint(90,30));

	new wxButton(panel, ID_Button_Confirm_AddFriend, _T("确认"), wxPoint(60,100));
}

void CFriendAddFrame::OnConfirmFriendName( wxCommandEvent& event )
{
	wxString strAccount = m_input_TextCtrl->GetValue();
	if (strAccount.IsEmpty())
	{
		wxMessageBox(_T("好友账号不能为空"));
		return;
	}

	TSingleton<CClientGUI>::GetInstance().OnAddFriend(strAccount.c_str());

	Close(true);
}