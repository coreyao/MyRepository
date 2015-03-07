#include "ManagerServer.h"
#include "DumpGenerator.h"
using Core::Utility::CDumpGenerator;

#pragma comment(lib, "DbgHelp.lib") 

int main(char* argc[], int argv)
{
	::SetUnhandledExceptionFilter(CDumpGenerator::TopLevelFilter);

	TSingleton<CManagerServer>::GetInstance().Init();
	TSingleton<CManagerServer>::GetInstance().Startup();
	
	return 0;
}