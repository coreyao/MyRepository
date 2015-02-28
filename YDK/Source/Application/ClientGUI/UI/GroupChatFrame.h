#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_GROUPCHATFRAME_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_GROUPCHATFRAME_H__INCLUDED

#include <map>
#include "UI/UICommon.h"
#include "XString/XString.h"

using Core::Utility::XString;
using Core::Utility::uint32;
using Core::Utility::int32;

class CGroupChatFrame : public wxFrame
{
public:
	CGroupChatFrame(uint32 unGroupID, const XString& strTitle, wxWindow* parent);
	void OnSend(wxCommandEvent& event);
	void OnRecv(uint32 unGroupMemberID, const XString& strContent, time_t nowTime);
	void OnClose(wxCloseEvent& event);
	void OnShowChatHistory(wxCommandEvent& event);

	void UpdateMemberList(uint32 unMemberID);

private:
	void _InitUI();
	void _InitMemberList();

private:
	wxPanel* m_panel;
	wxTextCtrl* m_output_TextCtrl;
	wxTextCtrl* m_input_TextCtrl;
	wxButton* m_send_button;
	wxButton* m_history_button;
	wxListBox* m_memberList;

	uint32 m_unGroupID;
	std::map<uint32, int32> m_mapMemberItems;

	DECLARE_EVENT_TABLE();
};

#endif