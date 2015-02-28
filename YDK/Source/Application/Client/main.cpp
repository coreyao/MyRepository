#include "ClientCMD.h"
#include "DumpGenerator.h"

using Core::Utility::CDumpGenerator;

#pragma comment(lib, "DbgHelp.lib") 

int main(char* argc[], int argv)
{
	::SetUnhandledExceptionFilter(CDumpGenerator::TopLevelFilter);

	TSingleton<CClientCMD>::GetInstance().Init();
	TSingleton<CClientCMD>::GetInstance().Startup();

	system("pause");
	return 0;
}