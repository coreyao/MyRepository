#include "msini/CMSINIReader.h"
#include <Windows.h>

BEGIN_NAMESPACE(Core)
BEGIN_NAMESPACE(Utility)

CMSINIReader::CMSINIReader()
{
}

CMSINIReader::~CMSINIReader()
{
}

Core::Utility::int32 CMSINIReader::GetFieldIntValue( const XString& strSection, const XString& strField )
{
	return ::GetPrivateProfileInt(strSection.c_str(), strField.c_str(), 0, m_strFileName.c_str());
}

XString CMSINIReader::GetFieldStringValue( const XString& strSection, const XString& strField )
{
	XChar tempStr[256] = {0};
	::GetPrivateProfileString(strSection.c_str(), strField.c_str(), TEXT(""), tempStr, sizeof(tempStr), m_strFileName.c_str());

	return XString(tempStr);
}

void CMSINIReader::SetFileName( const XString& fileName )
{
	m_strFileName = fileName;
}

END_NAMESPACE(Utility)
END_NAMESPACE(Core)
