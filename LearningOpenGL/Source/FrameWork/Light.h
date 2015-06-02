#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CMaterial
{
public:
	CMaterial();

	void SetBaseColorTexture(const std::string& sFileName);
	void SetShininess(float fShininess);

private:
	GLint m_baseColorTex;
	float m_fShininess;
};

enum ELightType
{
	ELightType_DirectionalLight,
	ELightType_PointLight,
	ELightType_SpotLight,
};

class CLightBase
{
protected:
	CLightBase()
		: m_eLightType(ELightType_DirectionalLight)
		, m_lightDir(Vec3(0, 0, -1))
		, m_bOn(true)
	{
	}

	ELightType m_eLightType;
	Vec3 m_lightDir;
	bool m_bOn;
};

class CDirectionalLight : public CLightBase
{
public:
	CDirectionalLight();
};

class CLightManager
{
public:
	static CLightManager* GetInstance();

	void AddLight(CLightBase* pLight);
	const std::vector<CLightBase*>& GetAllLights();

private:
	static CLightManager* s_pInstance;
	std::vector<CLightBase*> m_vAllLight;
};