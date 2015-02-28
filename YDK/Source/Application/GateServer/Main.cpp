#include "GateServer.h"
#include "DumpGenerator.h"

using Core::Utility::CDumpGenerator;

#pragma comment(lib, "DbgHelp.lib") 

int main(char* argc[], int argv)
{
	::SetUnhandledExceptionFilter(CDumpGenerator::TopLevelFilter);

	TSingleton<CGateServer>::GetInstance().Init();
	TSingleton<CGateServer>::GetInstance().Startup();
	return 0;
}