#include "FileGuageFrame.h"


CFileGuageFrame::CFileGuageFrame(const XString& strFileName)
	: wxFrame( NULL, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, (wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
{
	_InitUI();
	XString strTitle = strFileName;
	SetTitle(strTitle.c_str());
}

void CFileGuageFrame::_InitUI()
{
	wxColour col;
	col.Set("light blue");
	wxPanel* panel = new wxPanel(this,wxID_ANY);
	panel->SetBackgroundColour(col);

	m_gauge = new wxGauge(panel, wxID_ANY, 100, wxPoint(30,30), wxSize(300,30));
}

void CFileGuageFrame::OnUpdateGuage( uint32 percent )
{
	m_gauge->SetValue(percent);
}