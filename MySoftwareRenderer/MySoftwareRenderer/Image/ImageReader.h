#pragma once

#include <string>

class CImageReader
{
public:
	enum EImageType
	{
		EImageType_RGB8888,
		EImageType_RGB888,
	};

	CImageReader();
	virtual ~CImageReader();

	virtual void ReadData(const std::string& sFileName) = 0;
	unsigned char* GetData();
	float GetWidth();
	float GetHeight();
	EImageType GetImageType();

protected:
	void ResetData();

	unsigned char* m_pData;
	float m_fWidth;
	float m_fHeight;
	EImageType m_eType;
};