#include "FileIO/FileBase.h"
#include "XString/XString.h"

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

#if 0
using Core::Utility::XString;

namespace
{
    size_t CalculateFileLength(FILE* file)
    {
        int orignPosition = ftell(file);
        assert(orignPosition != -1);

        int ret = fseek(file, 0, SEEK_END);
        assert(ret == 0);

        int size = ftell(file);
        assert(size != -1);

        ret =fseek(file, orignPosition, SEEK_SET);
        assert(ret == 0);

        return size;
    }
}

FileBase::FileBase()
: mFile(NULL)
, mIsOpened(false)
{
}

FileBase::~FileBase()
{
}

bool FileBase::IsOpened()
{
    return mIsOpened;
}

void FileBase::Open(const XString &fileName, const XString& fileModeString)
{
#ifdef UNICODE
    mFile = _wfopen(fileName.c_str(), fileModeString.c_str());   
#else
    mFile = fopen(fileName.c_str(), fileModeString.c_str());
#endif

    if (mFile == NULL)
    {
        assert(mFile != NULL);
        mIsOpened = false;
    }
    else
    {
        mIsOpened = true;
    }

    mLen = CalculateFileLength(mFile);
}

void FileBase::Seek(size_t pos)
{
    int ret = fseek(mFile, pos, SEEK_SET);
    assert(ret == 0);
}

void FileBase::Close()
{
    int ret = fclose(mFile);
    assert(ret == 0);
}

size_t FileBase::GetLength()
{
    return mLen;
}

bool FileBase::IsEOF()
{
    return feof(mFile) != 0 ? true : false;
}
#endif

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
