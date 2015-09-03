#pragma once

class CImageManager
{
public:
	static CImageManager* GetInstance();
	void Load(const char* pFileName, unsigned char*& pOutData, float& fOutWidth, float& fOutHeight);
private:
	static CImageManager* s_pInstance;
};