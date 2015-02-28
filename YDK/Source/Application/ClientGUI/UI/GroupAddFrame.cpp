#include "UI/GroupAddFrame.h"
#include "ClientGUI/ClientGUI.h"

BEGIN_EVENT_TABLE(CGroupAddFrame, wxFrame)
	EVT_BUTTON(ID_Button_Confirm_AddGroup, CGroupAddFrame::OnConfirmgGroupName)
END_EVENT_TABLE()

CGroupAddFrame::CGroupAddFrame()
: wxFrame( NULL, wxID_ANY, _T("���Ⱥ"), wxDefaultPosition, wxSize(250, 200), (wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
{
	_InitUI();
}

void CGroupAddFrame::_InitUI()
{
	wxColour col;
	col.Set("light blue");
	wxPanel* panel = new wxPanel(this,wxID_ANY);
	panel->SetBackgroundColour(col);

	new wxStaticText(panel, wxID_ANY, _T("Ⱥ�˺ţ� "), wxPoint(30,30));
	m_input_TextCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxPoint(90,30));

	new wxButton(panel, ID_Button_Confirm_AddGroup, _T("ȷ��"), wxPoint(60,100));
}

void CGroupAddFrame::OnConfirmgGroupName( wxCommandEvent& event )
{
	wxString strAccount = m_input_TextCtrl->GetValue();
	if (strAccount.IsEmpty())
	{
		wxMessageBox(_T("Ⱥ�˺Ų���Ϊ��"));
		return;
	}

	TSingleton<CClientGUI>::GetInstance().OnAddGroup(strAccount.c_str());

	Close(true);
}
