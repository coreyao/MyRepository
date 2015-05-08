#include "Utility.h"

int gettimeofday( struct timeval * val, struct timezone * )
{
	if (val)
	{
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency( &liFreq );
		QueryPerformanceCounter( &liTime );
		val->tv_sec     = (long)( liTime.QuadPart / liFreq.QuadPart );
		val->tv_usec    = (long)( liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0 );
	}
	return 0;
}

extern char* GB2312ToUTF8( const char* gb2312 )
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);  
	wchar_t* wstr = new wchar_t[len+1];  
	memset(wstr, 0, len+1);  
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);  
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);  
	char* str = new char[len+1];  
	memset(str, 0, len+1);  
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);  
	if(wstr) delete[] wstr;  
	return str;  
}
