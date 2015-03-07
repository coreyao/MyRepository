#ifndef CORE_UTILITY_FILEREADER_H__INCLUDED
#define CORE_UTILITY_FILEREADER_H__INCLUDED

#include "FileIO/FileBase.h"

namespace Core{namespace Utility{class XString;}}

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class FileReader : public FileBase
{
public:
	enum E_FileEncoding
	{
		FILE_ENCODING_ANSI     = 0x01,
		FILE_ENCODING_UTF_16_L = 0x02,
		FILE_ENCODING_UTF_16_B = 0x04,
		FILE_ENCODING_UTF_8    = 0x08,
		FILE_ENCODING_BINARY   = 0x10,

		FILE_ENCODING_FORCETO32BITS = 0xffffffff
	};

	FileReader();
	FileReader(const XString& fileName, int fileEncoding);
	~FileReader();

	void    Open(const XString& fileName, int fileEncoding);

	//Read unformatted string
	size_t  Read(char* s, size_t len);

	//Read a line which is a formatted string
	size_t  ReadLine(XChar* s, const size_t len);


};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)


#endif