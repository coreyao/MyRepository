#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CMaterial
{
public:
	CMaterial();

	GLuint GetBaseColorTex();
	void SetBaseColorTexture(const std::string& sFileName);
	void SetShininess(float fShininess);

private:
	GLuint m_baseColorTex;
	float m_fShininess;
};

enum ELightType
{
	ELightType_DirectionalLight,
	ELightType_PointLight,
	ELightType_SpotLight,
};

class CMesh;
class CLightBase
{
public:
	CLightBase()
		: m_eLightType(ELightType_DirectionalLight)
		, m_lightDir(Vec3(0, 0, -1))
		, m_bOn(true)
		, m_pDebugMesh(nullptr)
	{
	}

	Vec3 m_ambientColor;
	Vec3 m_diffuseColor;
	Vec3 m_specularColor;

	ELightType m_eLightType;
	Vec3 m_lightDir;
	Vec3 m_lightPos;
	bool m_bOn;

	CMesh* m_pDebugMesh;
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