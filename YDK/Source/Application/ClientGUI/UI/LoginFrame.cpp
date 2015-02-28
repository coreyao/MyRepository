#include "UI/LoginFrame.h"
#include "Singleton.h"
#include "ClientGUI/ClientGUI.h"
#include "UI/MainFrame.h"
#include "ClientGUI/ClientPlayer.h"
#include "UI/RegisterFrame.h"

using Core::Utility::TSingleton;

DEFINE_EVENT_TYPE(EVT_COMMAND_CREATE_MAIN_FRAME)

BEGIN_EVENT_TABLE(CLoginFrame, wxFrame)
	EVT_BUTTON(ID_LoginButton, CLoginFrame::OnLogin)
	EVT_BUTTON(ID_RegisterButton, CLoginFrame::OnRegister)
	EVT_COMMAND(ID_LoginFrame, EVT_COMMAND_CREATE_MAIN_FRAME, CLoginFrame::OnLoginSuccess)
	EVT_CLOSE(CLoginFrame::OnClose)
END_EVENT_TABLE()

CLoginFrame::CLoginFrame()
	: wxFrame( NULL, ID_LoginFrame, _T(""), wxDefaultPosition, wxSize(170, 150), (wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
{
	_InitUI();
}

void CLoginFrame::_InitUI()
{
	wxColour col;
	col.Set(_T("light blue"));
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	panel->SetBackgroundColour(col);

	wxStaticText* stAccount = new wxStaticText(panel, wxID_ANY, _T("’À∫≈£∫"));
	wxStaticText* stPassword = new wxStaticText(panel, wxID_ANY, _T("√‹¬Î£∫"));

	m_CBAccount = new wxTextCtrl(panel, wxID_ANY);
	m_TCPassword = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);

	wxBoxSizer* topsizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* accountSizer = new wxBoxSizer(wxHORIZONTAL);
	accountSizer->Add(stAccount);
	accountSizer->Add(m_CBAccount);

	wxBoxSizer* passwordSizer = new wxBoxSizer(wxHORIZONTAL);
	passwordSizer->Add(stPassword);
	passwordSizer->Add(m_TCPassword);

	wxButton* loginBtn = new wxButton(panel, ID_LoginButton, _T("µ«¬º"));
	wxButton* registerBtn = new wxButton(panel, ID_RegisterButton, _T("◊¢≤·"));
	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->Add(loginBtn);
	buttonSizer->Add(registerBtn);

	topsizer->Add(accountSizer);
	topsizer->Add(passwordSizer);
	topsizer->AddSpacer(30);
	topsizer->Add(buttonSizer);
	
	panel->SetSizer(topsizer);
}

void CLoginFrame::OnLogin(wxCommandEvent& event)
{
	if (m_CBAccount->GetValue().IsEmpty())
	{
		wxMessageBox(_T("«Î ‰»Î’À∫≈"));
	}
	else if (m_TCPassword->GetValue().IsEmpty())
	{
		wxMessageBox(_T("«Î ‰»Î√‹¬Î"));
	}
	else
	{
		TSingleton<CClientGUI>::GetInstance().OnReqLogin(m_CBAccount->GetValue().c_str(), m_TCPassword->GetValue().c_str());
	}
}

void CLoginFrame::OnLoginSuccess( wxCommandEvent& event )
{
	TSingleton<CMainFrame>::GetInstance().SetTitle(event.GetString());
	TSingleton<CMainFrame>::GetInstance().Show();
	this->Hide();
}

void CLoginFrame::OnRegister( wxCommandEvent& event )
{
	CRegisterFrame* pFrame = new CRegisterFrame(this);
	pFrame->Show();
}

void CLoginFrame::OnClose( wxCloseEvent& event )
{
	TSingleton<CClientGUI>::GetInstance().Shutdown();
	Destroy();
}