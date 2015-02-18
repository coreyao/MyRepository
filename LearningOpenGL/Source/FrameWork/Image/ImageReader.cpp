#include "ImageReader.h"

CImageReader::CImageReader()
	: m_pData(nullptr)
{
}

CImageReader::~CImageReader()
{
	ResetData();
}

void CImageReader::ResetData()
{
	if ( m_pData )
		delete [] m_pData;
}

unsigned char* CImageReader::GetData()
{
	return m_pData;
}
