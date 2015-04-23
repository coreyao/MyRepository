#include "Utility.h"

#include "ft2build.h"
#include FT_FREETYPE_H

FT_Library library = nullptr;

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

void InitFreeType()
{
	FT_Error error = FT_Init_FreeType( &library );
	if ( error == FT_Err_Ok )
	{

	}
}
