#ifndef CORE_UTILITY_APPLICATION_CLIENTGUI_FILEGUAGEFRAME_H__INCLUDED
#define CORE_UTILITY_APPLICATION_CLIENTGUI_FILEGUAGEFRAME_H__INCLUDED

#include "UI/UICommon.h"
#include "XString/XString.h"

using Core::Utility::XString;
using Core::Utility::uint32;

class CFileGuageFrame : public wxFrame
{
public:
	CFileGuageFrame(const XString& strFileName);
	void OnUpdateGuage(uint32 percent);

private:
	void _InitUI();

private:
	wxGauge* m_gauge;
};

#endif