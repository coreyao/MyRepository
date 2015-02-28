#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_LOGINFRAME_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_LOGINFRAME_H__INCLUDED

#include "UI/UICommon.h"

class CLoginFrame : public wxFrame
{
public:
	CLoginFrame();
	void OnLogin(wxCommandEvent& event);
	void OnLoginSuccess(wxCommandEvent& event);
	void OnRegister(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

private:
	void _InitUI();

private:
	wxTextCtrl* m_CBAccount;
	wxTextCtrl* m_TCPassword;

	DECLARE_EVENT_TABLE()
};

#endif
