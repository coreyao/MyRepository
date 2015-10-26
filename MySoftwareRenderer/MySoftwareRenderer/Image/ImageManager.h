#pragma once

#include "../Utility.h"
#include "../DataTypes.h"

class CImageManager
{
public:
	static CImageManager* GetInstance();

	int Load(const char* pFileName);
	CTexture* FindTexture(int iID);

private:
	static CImageManager* s_pInstance;
	static int s_iNextID;

	map<int, CTexture> m_mAllTexture;
	map<string, int> m_mAllTextureFileName;
};