#pragma once

#include "Utility.h"
#include "DataTypes.h"

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

	Color4F m_ambientColor;
	Color4F m_diffuseColor;
	Color4F m_specularColor;

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

class CSpotLight : public CLightBase
{
public:
	CSpotLight();

	float fInnerAngle;
	float fOuterAngle;

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
	const std::vector<CSpotLight>& GetAllSpotLights();

private:
	static CLightManager* s_pInstance;

	CLightManager();

	std::vector<CDirectionalLight> m_vAllDirectionalLight;
	std::vector<CPointLight> m_vAllPointLight;
	std::vector<CSpotLight> m_vAllSpotLight;
};