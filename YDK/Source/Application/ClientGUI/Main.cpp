#include "UI/LoginFrame.h"
#include "UI/MainFrame.h"
#include "ClientGUI/ClientGUI.h"
#include "UI/GroupChatFrame.h"

class MyApp : public wxApp
{
	virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	TSingleton<CClientGUI>::GetInstance().Init();
	TSingleton<CClientGUI>::GetInstance().Startup(false);

	TSingleton<CLoginFrame>::GetInstance().Show(true);
	TSingleton<CMainFrame>::GetInstance().Hide();
	//TSingleton<CMainFrame>::GetInstance().Show(true);

	return true;
}

