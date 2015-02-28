#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_GROUPADDFRAME_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_GROUPADDFRAME_H__INCLUDED

#include "UI/UICommon.h"
#include "XString/XString.h"

using Core::Utility::XString;
using Core::Utility::uint32;

class CGroupAddFrame : public wxFrame
{
public:
	CGroupAddFrame();
	void OnConfirmgGroupName(wxCommandEvent& event);

private:
	void _InitUI();

private:
	wxTextCtrl* m_input_TextCtrl;

	DECLARE_EVENT_TABLE();
};

#endif