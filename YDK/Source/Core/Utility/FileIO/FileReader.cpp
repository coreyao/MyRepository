#include "FileIO/FileReader.h"
#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

#if 0
using Core::Utility::XString;

namespace
{
    bool isBinaryFile = true;
    bool isUnicodeFile = true;
}

FileReader::FileReader()
: FileBase()
{
}

FileReader::FileReader(const XString& fileName, int readMode)
: FileBase(fileName, readMode)
{
    Open(fileName, readMode);
}

FileReader::~FileReader()
{
}

void FileReader::Open(const XString& fileName, int fileMode)
{
    //FileReader should have an option "r" and option "w" is forbidden
    assert ((fileMode & FILE_MODE_NORMAL_READ) && !(fileMode & FILE_MODE_NORMAL_WRITE));

    //We are now reading a text file which is not empty
    if ((GetLength() != 0) && !(fileMode & FILE_MODE_BINARY))
    {
        wchar_t c;
        int ret = fread(&c, sizeof(wchar_t), 1, mFile);
        if (ret == 1)
        {
//UTF-16(little endian)
#ifdef WIN32
            //0xfeff is BOM(Byte Order Mark)
            if (c != 0xfeff)        
#else
            //linux (UTF-8)
#endif
            {
                isUnicodeFile = false;
            }
            
            rewind(mFile);
        }
        
        isBinaryFile = false;
    }
}

size_t FileReader::Read(XChar* s, size_t len)
{
    assert(s != NULL);

    int ret = 0;
    //If the file is empty, we do nothing
    if (GetLength() != 0)
    {
#ifdef UNICODE
        //If the file is not binary
        if (!isBinaryFile)
        {
            //ANSI FILE
            if (!isUnicodeFile)
            {
                char* temp = new char[len + 1]();
                ret = fread(temp, sizeof(char), len, mFile);
                if (ret != 0)
                {
                    XString::MultiCStringToWideCString(s, len, temp);
                }
                SAFE_DELETE_ARRAY(temp);
            }
            //UNICODE FILE
            else
            {
                ret = fread(s, sizeof(wchar_t), len, mFile);
            }
        }
        //If the file is binary, read directly
        else
        {
            ret = fread(s, sizeof(wchar_t), len, mFile);
        }

#else
        //If the file is not binary
        if (!isBinaryFile)
        {
            //ANSI FILE
            if (!isUnicodeFile)
            {
                ret = fread(s, sizeof(char), len, mFile);
            }
            //UNICODE FILE
            else
            {
                wchar_t* temp = new wchar_t[len]();
                ret = fread(temp, sizeof(wchar_t), len, mFile);
                if (ret != 0)
                {
                    XString::WideCStringToMultiCString(s, len, temp);
                }
                SAFE_DELETE_ARRAY(temp);
            }
        }
        //If the file is binary, read directly
        else
        {
            ret = fread(s, sizeof(char), len, mFile);
        }

#endif
    }

    return ret;
}

void FileReader::ReadLine(XChar* str, int len)
{
    assert(str != NULL);
#ifdef UNICODE
    XChar* result = fgetws(str, len, mFile);
#else
    XChar* result = fgets(str, len, mFile);
#endif
    assert(result != NULL);
}
#endif

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
