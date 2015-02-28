#ifndef CORE_UTILITY_UTILITYPES_H_INCLUDED
#define CORE_UTILITY_UTILITYPES_H_INCLUDED

#include <string>
#include <sstream>

namespace Core
{
namespace Utility
{

#ifdef UNICODE
    typedef wchar_t XChar;
    typedef std::wstring BasicString;
	typedef std::wostringstream StreamWriter;
	typedef std::wistringstream StreamReader;
#else
    typedef char XChar;
    typedef std::string BasicString;
	typedef std::ostringstream StreamWriter;
	typedef std::istringstream StreamReader;
#endif

	typedef unsigned int uint32;
	typedef int int32;
	typedef unsigned short uint16;
	typedef short int16;
	typedef unsigned char uint8;
	typedef char int8;
	typedef __int64 int64;
	typedef unsigned __int64 uint64;
}
}


#endif