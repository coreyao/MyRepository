#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_CHATHISTORYFRAME_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_CHATHISTORYFRAME_H__INCLUDED

#include "UI/UICommon.h"
#include "XString/XString.h"

using Core::Utility::XString;
using Core::Utility::uint32;

class CChatHistoryFrame : public wxFrame
{
public:
	CChatHistoryFrame(const XString& strName, wxWindow* parent);
	void OnShowSingleChatHistory(uint32 unPlayerID);
	void OnShowGroupChatHistory(uint32 unGroupID);

private:
	void _InitUI();

private:
	wxTextCtrl* m_output_TextCtrl;
};

#endif