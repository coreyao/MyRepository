#include "Light.h"
#include "Image/ImageManager.h"
#include "Mesh.h"

CDirectionalLight::CDirectionalLight()
	: CLightBase()
{
	m_eLightType = ELightType_DirectionalLight;
}

CLightManager* CLightManager::s_pInstance = nullptr;
CLightManager* CLightManager::GetInstance()
{
	if ( !s_pInstance )
	{
		s_pInstance = new CLightManager;
	}

	return s_pInstance;
}

void CLightManager::AddLight( CLightBase* pLight )
{
	m_vAllLight.push_back(pLight);
}

const std::vector<CLightBase*>& CLightManager::GetAllLights()
{
	return m_vAllLight;
}

CMaterial::CMaterial()
	: m_baseColorTex(-1)
	, m_fShininess(1)
{
}

void CMaterial::SetBaseColorTexture( const std::string& sFileName )
{
	unsigned char* pData = nullptr;
	float fWidth = 0;
	float fHeight = 0;

	CImageManager::GetInstance()->Load(sFileName.c_str(), pData, fWidth, fHeight);
	if ( pData )
	{
		glGenTextures(1, &m_baseColorTex);
		glBindTexture(GL_TEXTURE_2D, m_baseColorTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fWidth, fHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);

		glBindTexture(GL_TEXTURE_2D, 0);

		delete [] pData;
	}
}

void CMaterial::SetShininess( float fShininess )
{
	m_fShininess = fShininess;
}

GLuint CMaterial::GetBaseColorTex()
{
	return m_baseColorTex;
}
