#include "ImageManager.h"
#include "PNGReader.h"
#include "TGAReader.h"
#include <string>
#include <cctype>
#include <algorithm>

CImageManager* CImageManager::s_pInstance = nullptr;
CImageManager* CImageManager::GetInstance()
{
	if ( !s_pInstance )
	{
		s_pInstance = new CImageManager;
	}

	return s_pInstance;
}

int CImageManager::Load(const char* pFileName)
{
	CTexture outTexture;
	unsigned char* pOutData = nullptr;
	float fOutWidth = 0;
	float fOutHeight = 0;

	std::string sExt(pFileName);
	sExt = sExt.substr( sExt.find_last_of('.') + 1 );
	std::transform(sExt.begin(), sExt.end(), sExt.begin(), tolower);
	if ( sExt == "png" )
	{
		CPNGReader pngReader(pFileName);
		if ( pngReader.GetData() )
		{
			pOutData = pngReader.GetData();
			fOutWidth = pngReader.GetWidth();
			fOutHeight = pngReader.GetHeight();
		}
	}
	else if ( sExt == "tga" )
	{
		CTGAReader tgaReader(pFileName);
		if ( tgaReader.GetData() )
		{
			pOutData = tgaReader.GetData();
			fOutWidth = tgaReader.GetWidth();
			fOutHeight = tgaReader.GetHeight();
		}
	}

	outTexture.m_pData = pOutData;
	outTexture.m_iWidth = fOutWidth;
	outTexture.m_iHeight = fOutHeight;

	int iID = s_iNextID++;
	m_mAllTexture[iID] = outTexture;
	return iID;
}

const CTexture* CImageManager::FindTexture(int iID)
{
	auto it = m_mAllTexture.find(iID);
	if ( it != m_mAllTexture.end() )
		return &it->second;

	return nullptr;
}

int CImageManager::s_iNextID = 1;

