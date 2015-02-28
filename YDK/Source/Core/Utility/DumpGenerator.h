#ifndef CORE_UTILITY_DUMPGENERATOR_H__INCLUDED
#define CORE_UTILITY_DUMPGENERATOR_H__INCLUDED

#include <ctime>
#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

#ifndef DUMP_FILE_PATH
#define DUMP_FILE_PATH TEXT("..\\..\\..\\output\\%4d.%02d.%02d_%02d.%02d.%02d.dmp")
#endif

using Core::Utility::XString;

class CDumpGenerator
{
public:
	static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
	{
		LONG ret = EXCEPTION_CONTINUE_SEARCH;

		time_t nowtime = ::time(NULL);
		struct tm *pTime = NULL;
		localtime_s(pTime, &nowtime);

		XString filePath;
		filePath.Format(DUMP_FILE_PATH, pTime->tm_year+1900, pTime->tm_mon+1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);

		HANDLE hFile = ::CreateFile(filePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION ExInfo;

			ExInfo.ThreadId = ::GetCurrentThreadId();
			ExInfo.ExceptionPointers = pExceptionInfo;
			ExInfo.ClientPointers = NULL;

			// write the dump
			BOOL bOK = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
			ret = EXCEPTION_EXECUTE_HANDLER;
			::CloseHandle(hFile);
		}
		return ret;
	}
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif
