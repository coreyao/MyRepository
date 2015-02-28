#define  WIN32_LEAN_AND_MEAN
#include <cassert>
#include <cstdarg>
#include <Windows.h>

#include "XString/XString.h"
#include "TimeInfo/TimeInfo.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

namespace
{
    const size_t BUFFER_SIZE = 1024;
}

XString::XString()
: BasicString()
{
}

XString::XString(time_t seconds)
: BasicString()
{
//convert time_t to string
	*this = CTimeInfo(seconds).ToString();
}

XString::XString(const XString& str, size_t pos, size_t n)
: BasicString(str, pos, n)
{
}

XString::XString(const XChar* s, size_t n)
: BasicString(s, n)
{
}

XString::XString(const XChar* s)
: BasicString(s)
{
}

XString::XString(size_t n, XChar c)
: BasicString(n, c)
{
}

XString::XString(const XString& other)
: BasicString(other)
{
}

XString::XString(const BasicString& other)
: BasicString(other)
{
}

XString::~XString()
{
    //Call the destructor manually since the ~baic_string() is non-virtual
    BasicString::~basic_string();
}

XString& XString::Format(const XChar* format, ...)
{
    assert(format != NULL);

    XChar buffer[BUFFER_SIZE] = {0};
    va_list args;
    va_start(args, format);
    int ret = 0;
#ifdef UNICODE
    ret = _vsnwprintf(buffer, BUFFER_SIZE, format, args);
#else
    ret = _vsnprintf(buffer, BUFFER_SIZE, format, args);
#endif
    if (ret == -1)
    {
        assert(ret != -1);
    }
    else
    {
        *this = buffer;
    }

    return *this;
}

std::string XString::ToMultiCharString()
{
#ifdef UNICODE
    size_t textLen = WideCharToMultiByte(CP_ACP, 0, c_str(), -1, NULL, 0, NULL, NULL);
    assert(textLen != 0);

    char* elementText = new char[textLen + 1]();

    int ret = WideCharToMultiByte(CP_ACP, 0, c_str(), -1, elementText, textLen, NULL, NULL);
    assert(ret != 0);

    std::string result = elementText;
    SAFE_DELETE_ARRAY(elementText);

    return result;
#else
    return *this;
#endif
}

std::wstring XString::ToWideCharString()
{
#ifndef UNICODE
    int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, c_str(), -1, NULL, 0);
    assert(unicodeLen != 0);

    wchar_t* pUnicode = new  wchar_t[unicodeLen+1]();
    int ret = MultiByteToWideChar(CP_UTF8, 0, c_str(), -1, pUnicode, unicodeLen);  
    assert(ret != 0);

    std::wstring result = pUnicode; 
    SAFE_DELETE_ARRAY(pUnicode);

    return  result;
#else
    return *this;
#endif
}

void XString::WideCStringToMultiCString(char* multiCString, size_t multiCStringLen, const wchar_t* wideCString)
{
    size_t textLen = ::WideCharToMultiByte(CP_ACP, 0, wideCString, -1, NULL, 0, NULL, NULL);
    assert(multiCStringLen >= textLen);

    int ret = ::WideCharToMultiByte(CP_ACP, 0, wideCString, -1, multiCString, textLen, NULL, NULL);
    assert(ret != 0);
}

void XString::MultiCStringToWideCString(wchar_t* wideCString, size_t wideCStringLen, const char* multiCString)
{
    size_t unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, multiCString, -1, NULL, 0);
    assert(wideCStringLen >= unicodeLen);

    int ret = ::MultiByteToWideChar(CP_ACP, 0, multiCString, -1, wideCString, unicodeLen);  
    assert(ret != 0);
}

XString& XString::operator=(const XString& other)
{
    if (this != &other)
    {
        BasicString::operator=(other);
    }

    return *this;
}

XString& XString::operator=(const BasicString& other)
{
    if (this != &other)
    {
        BasicString::operator=(other);
    }

    return *this;
}

XString& XString::operator=(const XChar* s)
{
  
    BasicString::operator=(s);
    return *this;
}

XString& XString::operator=(XChar c)
{
    BasicString::operator=(c);

    return *this;
}

int XString::ConvertToInt32()
{
    int result = 0;
    StreamReader iss(*this);
    iss >> result;

    return result;
}

unsigned int XString::ConvertToUInt32()
{
    unsigned int result = 0;
    StreamReader iss(*this);
    iss >> result;

    return result;
}

__int64 XString::ConvertToUInt64()
{
    __int64 result = 0;
    StreamReader iss(*this);
    iss >> result;

    return result;
}

size_t XString::GetStringLength(const XChar* str)
{
#ifdef UNICODE
    return wcslen(str);
#else
    return strlen(str);
#endif
}

void XString::ToLower()
{
	uint8 delta = 'a' - 'A';
	XIterator iter = begin();
	for (; iter != end(); ++iter)
	{
		if (*iter >= 'A' && *iter <= 'Z')
		{
			*iter += delta;
		}
	}
}

void XString::ToUpper()
{
	uint8 delta = 'a' - 'A';
	XIterator iter = begin();
	for (; iter != end(); ++iter)
	{
		if (*iter >= 'a' && *iter <= 'z')
		{
			*iter -= delta;
		}
	}
}

std::vector<XString> XString::SplitString()
{
	std::vector<XString> result;
	XString str = *this;
	uint32 pos = str.find_first_of(' ');
	if (pos == XString::npos)
	{
		result.push_back(str);
		return result;
	}

	uint32 lastPos = 0;
	while (pos != XString::npos)
	{
		uint32 len = pos - lastPos;
		XString subString = str.substr(lastPos, len);
		result.push_back(subString);

		lastPos += (len + 1);
		pos = str.find_first_of(' ', lastPos);
	}

	if (lastPos > 0)
	{
		result.push_back(str.substr(lastPos));
	}
	
	return result;
}
END_NAMESPACE(Utility)
END_NAMESPACE(Core)