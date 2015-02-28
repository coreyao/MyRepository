#include "DBServer.h"
#include "DumpGenerator.h"
using Core::Utility::CDumpGenerator;

#pragma comment(lib, "DbgHelp.lib") 

int main(char* argc[], int argv)
{
	::SetUnhandledExceptionFilter(CDumpGenerator::TopLevelFilter);

	TSingleton<CDBServer>::GetInstance().Init();
	TSingleton<CDBServer>::GetInstance().Startup();

	return 0;
}