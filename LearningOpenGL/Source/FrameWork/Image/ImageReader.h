#pragma once

#include <string>

class CImageReader
{
public:
	CImageReader();
	virtual ~CImageReader();

	virtual void ReadData(const std::string& sFileName) = 0;
	unsigned char* GetData();
	float GetWidth();
	float GetHeight();

protected:
	void ResetData();

	unsigned char* m_pData;
	float m_fWidth;
	float m_fHeight;
};