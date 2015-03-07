#ifndef CORE_UTILITY_FILEWRITER_H__INCLUDED
#define CORE_UTILITY_FILEWRITER_H__INCLUDED

#include "FileIO/FileBase.h"

namespace Core{namespace Utility{class XString;}}

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class FileWriter : public FileBase
{
public:
	FileWriter();
	FileWriter(const XString& fileName, int writeMode);
	~FileWriter();

	void Open(const XString& fileName, int fileMode);
	size_t Write(const XChar* s, size_t len);

};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif