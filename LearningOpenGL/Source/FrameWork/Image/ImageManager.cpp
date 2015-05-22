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

void CImageManager::Load( const char* pFileName, unsigned char*& pOutData, float& fOutWidth, float& fOutHeight )
{
	pOutData = nullptr;

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
}

