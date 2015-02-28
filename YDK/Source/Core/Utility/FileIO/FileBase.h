#ifndef CORE_UTILITY_FILEBASE_H_INCLUDED
#define CORE_UTILITY_FILEBASE_H_INCLUDED

#include <stdio.h>
#include "BasicDefines.h"
#include "UtilityTypes.h"

namespace Core{namespace Utility{class XString;}}

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class FileBase
{
public:
	FileBase();
	~FileBase();

	void    Open(const XString& fileName, const XString& fileModeString);
	bool    IsOpened();
	size_t  GetLength();
	void    Seek(size_t pos);
	bool    IsEOF();
	void    Close();

protected:
	FILE* mFile;
	bool mIsOpened;
	size_t mLen;

private:
	FileBase(const FileBase&);
	FileBase& operator=(const FileBase&);

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif