#ifndef CORE_UTILITY_CMSINIREADER_H__INCLUDED
#define CORE_UTILITY_CMSINIREADER_H__INCLUDED

#include "XString/XString.h"

using Core::Utility::int32;

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

class CMSINIReader
{
public:
	CMSINIReader();
	~CMSINIReader();

	void SetFileName(const XString& fileName);

	int32 GetFieldIntValue(const XString& strSection, const XString& strField);
	XString GetFieldStringValue(const XString& strSection, const XString& strField);

private:
	XString m_strFileName;
};

END_NAMESPACE(Utility)
END_NAMESPACE(Core)

#endif