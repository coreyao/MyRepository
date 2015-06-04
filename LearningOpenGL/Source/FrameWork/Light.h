#pragma once

#include "Utility.h"
#include "OpenGL/GLFrameWork.h"

class CMaterial
{
public:
	CMaterial();

	GLuint GetBaseColorTex();
	float GetShininess();

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
	ELightType m_eLightType;

	Vec3 m_ambientColor;
	Vec3 m_diffuseColor;
	Vec3 m_specularColor;

	Vec3 m_lightDir;
	Vec3 m_lightPos;
	bool m_bOn;

	CMesh* m_pDebugMesh;

protected:
	CLightBase()
		: m_lightDir(Vec3(0, 0, -1))
		, m_bOn(true)
		, m_pDebugMesh(nullptr)
		, m_eLightType(ELightType_DirectionalLight)
	{
	}
};

class CDirectionalLight : public CLightBase
{
public:
	CDirectionalLight();
};

class CPointLight : public CLightBase
{
public:
	CPointLight();

	float m_attenuation_constant;
	float m_attenuation_linear;
	float m_attenuation_quadratic;
};

class CLightManager
{
public:
	static CLightManager* GetInstance();

	void AddLight(CLightBase* pLight);

	const std::vector<CDirectionalLight>& GetAllDirectionalLights();
	const std::vector<CPointLight>& GetAllPointLights();

	static const int conMaxDirectionalLightNum = 5;
	static const int conMaxPointLightNum = 5;

private:
	static CLightManager* s_pInstance;

	CLightManager();

	int m_iCurDirectionalLightNum;
	std::vector<CDirectionalLight> m_vAllDirectionalLight;

	int m_iCurPointLightNum;
	std::vector<CPointLight> m_vAllPointLight;
};