#include "Projector.h"
#include "Image/PNGReader.h"
#include "Image/ImageManager.h"

CProjector::CProjector(const std::string& sFileName)
{
	unsigned char* pData = nullptr;
	float fWidth = 0;
	float fHeight = 0;

	CImageManager::GetInstance()->Load(sFileName.c_str(), pData, fWidth, fHeight);
	if (pData)
	{
		glGenTextures(1, &m_colorTex);
		glBindTexture(GL_TEXTURE_2D, m_colorTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fWidth, fHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);

		glBindTexture(GL_TEXTURE_2D, 0);


		delete[] pData;
	}
}

void CProjector::Update(float dt)
{
}

void CProjector::Render()
{

}

