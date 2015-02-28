#include "UI/RegisterFrame.h"
#include "ClientGUI/ClientGUI.h"
#include "Singleton.h"

using Core::Utility::TSingleton;
using Core::Utility::uint8;

DEFINE_EVENT_TYPE(EVT_COMMAND_RECV_REGISTER_RESULT)

BEGIN_EVENT_TABLE(CRegisterFrame, wxFrame)
	EVT_BUTTON(ID_Register_Confirm, CRegisterFrame::OnConfirmRegister)
	EVT_BUTTON(ID_Register_Reset, CRegisterFrame::OnResetRegister)
	EVT_COMMAND(ID_Register_Frame, EVT_COMMAND_RECV_REGISTER_RESULT, CRegisterFrame::OnRecvRegisterResult)
END_EVENT_TABLE()

CRegisterFrame::CRegisterFrame( wxWindow* parent )
	: wxFrame(parent, ID_Register_Frame, _T(""), wxDefaultPosition, wxSize(300, 300), (wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
{
	_InitUI();
}

void CRegisterFrame::_InitUI()
{
	wxColour col;
	col.Set("light blue");
	m_panel = new wxPanel(this,wxID_ANY);
	m_panel->SetBackgroundColour(col);

	m_account_staticText = new wxStaticText(m_panel, wxID_ANY, _T("ÕËºÅ:"), wxPoint(20,20));
	m_password_staticText = new wxStaticText(m_panel, wxID_ANY, _T("ÃÜÂë:"), wxPoint(20,60));
	m_password2_staticText = new wxStaticText(m_panel, wxID_ANY, _T("ÃÜÂëÈ·ÈÏ:"), wxPoint(20,100));
	m_nickname_staticText = new wxStaticText(m_panel, wxID_ANY, _T("êÇ³Æ:"), wxPoint(20,140));

	m_account_textCtrl = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxPoint(80,20), wxSize(180,20));
	m_password_textCtrl = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxPoint(80,60), wxSize(180,20));
	m_password2_textCtrl = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxPoint(80,100), wxSize(180,20));
	m_nickname_textctrl = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString, wxPoint(80,140), wxSize(180,20));

	m_register_button = new wxButton(m_panel, ID_Register_Confirm, _T("È·ÈÏ×¢²á"), wxPoint(20,220));
	m_clear_button = new wxButton(m_panel, ID_Register_Reset, _T("ÖØÌî"), wxPoint(190,220));
}

void CRegisterFrame::OnConfirmRegister(  wxCommandEvent& event )
{
	if (m_account_textCtrl->IsEmpty() 
		|| m_password_textCtrl->IsEmpty()
		|| m_password2_textCtrl->IsEmpty()
		|| m_nickname_textctrl->IsEmpty()
		|| m_password_textCtrl->GetValue() != m_password2_textCtrl->GetValue())
	{
		wxMessageBox(_T("Êı¾İÌîĞ´´íÎó"));
		return;
	}

	TSingleton<CClientGUI>::GetInstance().OnReqRegister(m_account_textCtrl->GetValue().c_str(),
														m_password_textCtrl->GetValue().c_str(),
														m_nickname_textctrl->GetValue().c_str());
}

void CRegisterFrame::OnResetRegister(wxCommandEvent& event )
{
	m_account_textCtrl->Clear();
	m_password_textCtrl->Clear();
	m_password2_textCtrl->Clear();
	m_nickname_textctrl->Clear();
}

void CRegisterFrame::OnRecvRegisterResult( wxCommandEvent& event )
{
	uint8 result = static_cast<uint8>(event.GetInt());
	if (result == MsgAckRegisterAccount::EResult_Success)
	{
		wxMessageBox(_T("×¢²á³É¹¦"));
	}
	else
	{
		wxMessageBox(_T("×¢²áÊ§°Ü"));
	}
}