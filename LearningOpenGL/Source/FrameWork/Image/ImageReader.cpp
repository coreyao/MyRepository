#include "ImageReader.h"

CImageReader::CImageReader()
	: m_pData(nullptr)
	, m_fWidth(0)
	, m_fHeight(0)
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

float CImageReader::GetWidth()
{
	return m_fWidth;
}

float CImageReader::GetHeight()
{
	return m_fHeight;
}
