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
	if (!s_pInstance)
	{
		s_pInstance = new CLightManager;
	}

	return s_pInstance;
}

void CLightManager::AddLight(CLightBase* pLight)
{
	switch (pLight->m_eLightType)
	{
	case ELightType_DirectionalLight:
		m_vAllDirectionalLight.push_back(*(CDirectionalLight*)pLight);
		break;
	case ELightType_PointLight:
		m_vAllPointLight.push_back(*(CPointLight*)pLight);
		break;
	case ELightType_SpotLight:
		m_vAllSpotLight.push_back(*(CSpotLight*)pLight);
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
