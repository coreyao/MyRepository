#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_FRIENDGROUPCREATEFRAME_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_FRIENDGROUPCREATEFRAME_H__INCLUDED

#include "UI/UICommon.h"
#include "XString/XString.h"

using Core::Utility::XString;
using Core::Utility::uint32;

class CFriendGroupCreateFrame : public wxFrame
{
public:
	CFriendGroupCreateFrame();
	void OnConfirmgCreateFriendGroupName(wxCommandEvent& event);

private:
	void _InitUI();

private:
	wxTextCtrl* m_input_TextCtrl;

	DECLARE_EVENT_TABLE();
};

#endif