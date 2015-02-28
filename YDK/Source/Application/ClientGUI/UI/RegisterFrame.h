#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_REGISTERFRAME_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_REGISTERFRAME_H__INCLUDED

#include "UI/UICommon.h"
#include "XString/XString.h"

using Core::Utility::XString;
using Core::Utility::uint32;

class CRegisterFrame : public wxFrame
{
public:
	CRegisterFrame(wxWindow* parent = NULL);
	void OnConfirmRegister(wxCommandEvent& event);
	void OnResetRegister(wxCommandEvent& event);
	void OnRecvRegisterResult(wxCommandEvent& event);

private:
	void _InitUI();

	wxPanel* m_panel;
	wxStaticText* m_account_staticText;
	wxStaticText* m_password_staticText;
	wxStaticText* m_password2_staticText;
	wxStaticText* m_nickname_staticText;

	wxTextCtrl* m_account_textCtrl;
	wxTextCtrl* m_password_textCtrl;
	wxTextCtrl* m_password2_textCtrl;
	wxTextCtrl* m_nickname_textctrl;

	wxButton* m_register_button;
	wxButton* m_clear_button;

	DECLARE_EVENT_TABLE();
};

#endif