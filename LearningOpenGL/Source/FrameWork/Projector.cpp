#include "Projector.h"
#include "Image/PNGReader.h"
#include "Image/ImageManager.h"

CProjector::CProjector(const std::string& sFileName)
	: FOV(90)
	, AspectRatio((float)SCREEN_WIDTH / SCREEN_HEIGHT)
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
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		delete[] pData;
	}

	ProjectionMat = Mat4::CreateOrthegraphicsMat(-100, 100, 100, -100, 1.0, 1000.0);
}

void CProjector::Update(float dt)
{
}
