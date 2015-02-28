#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_FRIENDCHATFRAME_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_FRIENDCHATFRAME_H__INCLUDED

#include "UI/UICommon.h"
#include "XString/XString.h"

using Core::Utility::XString;
using Core::Utility::uint32;

class CFriendChatFrame : public wxFrame
{
public:
	CFriendChatFrame(uint32 unFriendID, const XString& strTitle, wxWindow* parent);
	void OnSend(wxCommandEvent& event);
	void OnRecv(const XString& strContent, time_t nowTime);
	void OnClose(wxCloseEvent& event);
	void OnShowChatHistory(wxCommandEvent& event);

	uint32 GetPlayerID() const { return m_unPlayerID; }
	void SetPlayerID(uint32 val) { m_unPlayerID = val; }

private:
	void _InitUI();

private:
	wxPanel* m_panel;
	wxTextCtrl* m_output_TextCtrl;
	wxTextCtrl* m_input_TextCtrl;
	wxButton* m_send_button;
	wxButton* m_history_button;

	uint32 m_unPlayerID;

	DECLARE_EVENT_TABLE();
};

#endif