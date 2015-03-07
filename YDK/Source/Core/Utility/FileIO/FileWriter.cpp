#include "FileIO/FileWriter.h"
#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

#if 0
using Core::Utility::XString;

FileWriter::FileWriter()
: FileBase()
{
}

FileWriter::FileWriter( const XString& fileName, int writeMode )
: FileBase(fileName, writeMode)
{
    Open(fileName, writeMode);
}

FileWriter::~FileWriter()
{
}

void FileWriter::Open(const XString& fileName, int fileMode)
{
    //FileReader should have an option "w" and option "r" is forbidden
    assert ((fileMode & FILE_MODE_NORMAL_WRITE) && !(fileMode & FILE_MODE_NORMAL_READ));

    FileBase::Open(fileName, fileMode);
//UTF-16(little endian)
#ifdef WIN32
    #ifdef UNICODE
        if ((fileMode & FILE_MODE_NORMAL_WRITE) && !(fileMode & FILE_MODE_BINARY))
        {
            wchar_t BOM = 0xfeff;
            int ret = fwrite(&BOM, sizeof(wchar_t), 1, mFile);
            assert(ret == 1);
        }
    #endif
#endif
}

size_t FileWriter::Write(const XChar* s, size_t len)
{
    assert(s != NULL);
    int ret = fwrite(s, sizeof(XChar), len, mFile);
    assert(ret == len);
    return ret;
}

#endif

END_NAMESPACE(Utility)
END_NAMESPACE(Core)