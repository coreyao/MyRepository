#include "UI/ChatHistoryFrame.h"
#include "Singleton.h"
#include "ClientGUI/ChatHistoryManager.h"
#include "TimeInfo/TimeInfo.h"

using Core::Utility::TSingleton;
using Core::Utility::CTimeInfo;

CChatHistoryFrame::CChatHistoryFrame( const XString& strName, wxWindow* parent)
	: wxFrame(parent, wxID_ANY, _T(""), wxDefaultPosition, wxSize(300,330), (wxDEFAULT_FRAME_STYLE & ~wxMAXIMIZE_BOX ))
{
	_InitUI();
	XString strTitle = _T("ÁÄÌì¼ÇÂ¼(") + strName + _T(")");
	this->SetTitle(strTitle.c_str());
}

void CChatHistoryFrame::OnShowSingleChatHistory(uint32 unPlayerID)
{
	m_output_TextCtrl->Clear();

	CChatHistoryManager::LSTHISTORYLIST* pHistoryList = TSingleton<CChatHistoryManager>::GetInstance().GetPlayerHistory(unPlayerID);
	if (pHistoryList != NULL)
	{
		CChatHistoryManager::LSTHISTORYLIST::iterator historyIter = pHistoryList->begin();
		for (; historyIter != pHistoryList->end(); ++historyIter)
		{
			time_t time = historyIter->m_unTime;
			XString strMsg = historyIter->m_strSenderName;
			strMsg += _T(" ");
			strMsg += CTimeInfo(time).ToString();
			strMsg += _T("\n");
			strMsg += historyIter->m_strContent;
			strMsg += _T("\n");

			m_output_TextCtrl->AppendText(strMsg);
		}
	}
}

void CChatHistoryFrame::OnShowGroupChatHistory( uint32 unGroupID )
{
	m_output_TextCtrl->Clear();

	CChatHistoryManager::LSTHISTORYLIST* pHistoryList = TSingleton<CChatHistoryManager>::GetInstance().GetGroupHistory(unGroupID);
	if (pHistoryList != NULL)
	{
		CChatHistoryManager::LSTHISTORYLIST::iterator historyIter = pHistoryList->begin();
		for (; historyIter != pHistoryList->end(); ++historyIter)
		{
			time_t time = historyIter->m_unTime;
			XString strMsg = historyIter->m_strSenderName;
			strMsg += _T(" ");
			strMsg += CTimeInfo(time).ToString();
			strMsg += _T("\n");
			strMsg += historyIter->m_strContent;
			strMsg += _T("\n");

			m_output_TextCtrl->AppendText(strMsg);
		}
	}
}

void CChatHistoryFrame::_InitUI()
{
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	m_output_TextCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(290, 290), wxTE_MULTILINE | wxTE_READONLY);
}