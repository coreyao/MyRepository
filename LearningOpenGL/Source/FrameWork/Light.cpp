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
		s_pInstance->m_vAllDirectionalLight.resize(conMaxDirectionalLightNum);
		s_pInstance->m_vAllPointLight.resize(conMaxPointLightNum);
		s_pInstance->m_vAllSpotLight.resize(conMaxSpotLightNum);
	}

	return s_pInstance;
}

void CLightManager::AddLight( CLightBase* pLight )
{
	switch (pLight->m_eLightType)
	{
	case ELightType_DirectionalLight:
		{
			if ( m_iCurDirectionalLightNum < conMaxDirectionalLightNum )
				m_vAllDirectionalLight[m_iCurDirectionalLightNum++] = *(CDirectionalLight*)pLight;
		}
		break;
	case ELightType_PointLight:
		{
			if ( m_iCurPointLightNum < conMaxDirectionalLightNum )
				m_vAllPointLight[m_iCurPointLightNum++] = *(CPointLight*)pLight;
		}
		break;
	case ELightType_SpotLight:
		{
			if ( m_iCurSpotLightNum < conMaxSpotLightNum )
				m_vAllSpotLight[m_iCurSpotLightNum++] = *(CSpotLight*)pLight;
		}
		break;
	default:
		break;
	}
}

const std::vector<CDirectionalLight>& CLightManager::GetAllDirectionalLights()
{
	return m_vAllDirectionalLight;
}

const std::vector<CPointLight>& CLightManager::GetAllPointLights()
{
	return m_vAllPointLight;
}

const std::vector<CSpotLight>& CLightManager::GetAllSpotLights()
{
	return m_vAllSpotLight;
}

CLightManager::CLightManager()
	: m_iCurDirectionalLightNum(0)
	, m_iCurPointLightNum(0)
	, m_iCurSpotLightNum(0)
{
}

CMaterial::CMaterial()
	: m_baseColorTex(-1)
	, m_fShininess(32)
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

float CMaterial::GetShininess()
{
	return m_fShininess;
}

CPointLight::CPointLight()
	: CLightBase()
	, m_attenuation_constant(1.0f)
	, m_attenuation_linear(0.0f)
	, m_attenuation_quadratic(0.0f)
{
	m_eLightType = ELightType_PointLight;
}

CSpotLight::CSpotLight()
	: CLightBase()
	, m_attenuation_constant(1.0f)
	, m_attenuation_linear(0.0f)
	, m_attenuation_quadratic(0.0f)
	, fInnerAngle(30.0f)
	, fOuterAngle(60.0f)
{
	m_eLightType = ELightType_SpotLight;
}
