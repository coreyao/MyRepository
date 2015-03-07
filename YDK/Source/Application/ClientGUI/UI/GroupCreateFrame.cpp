#include "UI/GroupCreateFrame.h"
#include "ClientGUI/ClientGUI.h"

BEGIN_EVENT_TABLE(CGroupCreateFrame, wxFrame)
	EVT_BUTTON(ID_Button_Confirm_CreateGroup, CGroupCreateFrame::OnConfirmgCreateGroupName)
END_EVENT_TABLE()

CGroupCreateFrame::CGroupCreateFrame()
	: wxFrame( NULL, wxID_ANY, _T("创建群"), wxDefaultPosition, wxSize(250, 200),(wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
{
	_InitUI();
}

void CGroupCreateFrame::OnConfirmgCreateGroupName( wxCommandEvent& event )
{
	wxString strName = m_input_TextCtrl->GetValue();
	if (strName.IsEmpty())
	{
		wxMessageBox(_T("群名称不能为空"));
		return;
	}

	TSingleton<CClientGUI>::GetInstance().OnCreateGroup(strName.c_str());

	Close(true);
}

void CGroupCreateFrame::_InitUI()
{
	wxColour col;
	col.Set("light blue");
	wxPanel* panel = new wxPanel(this,wxID_ANY);
	panel->SetBackgroundColour(col);

	new wxStaticText(panel, wxID_ANY, _T("群名称： "), wxPoint(30,30));
	m_input_TextCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxPoint(90,30));

	new wxButton(panel, ID_Button_Confirm_CreateGroup, _T("确认"), wxPoint(60,100));
}