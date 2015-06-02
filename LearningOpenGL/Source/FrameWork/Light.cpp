#include "Light.h"

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
		
}

void CMaterial::SetShininess( float fShininess )
{
	m_fShininess = fShininess;
}
