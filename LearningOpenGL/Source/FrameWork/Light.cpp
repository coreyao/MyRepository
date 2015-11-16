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

void CLightManager::Purge()
{
	delete s_pInstance;
}

void CLightManager::AddLight(CLightBase* pLight)
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
	, fInnerAngle(0.0f)
	, fOuterAngle(0.0f)
{
	m_eLightType = ELightType_SpotLight;
}
