#include "DeferredShading.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"
#include "Light.h"
#include "Mesh.h"
#include <sstream>

void CDeferredShading::Init()
{
	glGenFramebuffers(1, &m_GBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);

	// - Position color buffer
	glGenTextures(1, &m_PositionTex);
	glBindTexture(GL_TEXTURE_2D, m_PositionTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PositionTex, 0);

	// - Normal color buffer
	glGenTextures(1, &m_NormalTex);
	glBindTexture(GL_TEXTURE_2D, m_NormalTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_NormalTex, 0);

	// - Color + Specular color buffer
	glGenTextures(1, &m_DiffuseTex);
	glBindTexture(GL_TEXTURE_2D, m_DiffuseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_DiffuseTex, 0);

	// - Create and attach depth buffer (renderbuffer)
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SCommonVertex vVertex[4] = {
		SCommonVertex(Vec3(-1, -1, 0), Color4F(1, 1, 1, 1), Vec2(0, 0)),
		SCommonVertex(Vec3(-1, 1, 0), Color4F(1, 1, 1, 1), Vec2(0, 1)),
		SCommonVertex(Vec3(1, 1, 0), Color4F(1, 1, 1, 1), Vec2(1, 1)),
		SCommonVertex(Vec3(1, -1, 0), Color4F(1, 1, 1, 1), Vec2(1, 0))
	};

	unsigned short vVertexIndex[6] = {
		0, 1, 2, 
		0, 2, 3
	};

	glGenBuffers(1, &m_vertexDataObj);
	glGenBuffers(1, &m_vertexIndexObj);
	glGenVertexArrays(1, &m_vertexAttributeObj);
	glBindVertexArray(m_vertexAttributeObj);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(SCommonVertex), vVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), vVertexIndex, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_UV));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	m_GeometryPassProgram = CGLProgramManager::GetInstance()->CreateProgramByName("GBuffer");
	m_LightingPassProgram = CGLProgramManager::GetInstance()->CreateProgramByName("DeferredShading");
}

void CDeferredShading::PreRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CDeferredShading::PostRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(m_LightingPassProgram);
	UpdateLightUniform();

	GLint colorTextureUnif = glGetUniformLocation(m_LightingPassProgram, "gPosition");
	if (colorTextureUnif >= 0)
	{
		glUniform1i(colorTextureUnif, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_PositionTex);
		glBindSampler(0, m_Sampler);
	}

	colorTextureUnif = glGetUniformLocation(m_LightingPassProgram, "gNormal");
	if (colorTextureUnif >= 0)
	{
		glUniform1i(colorTextureUnif, 1);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, m_NormalTex);
		glBindSampler(1, m_Sampler);
	}

	colorTextureUnif = glGetUniformLocation(m_LightingPassProgram, "gAlbedoSpec");
	if (colorTextureUnif >= 0)
	{
		glUniform1i(colorTextureUnif, 2);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, m_DiffuseTex);
		glBindSampler(2, m_Sampler);
	}

	RenderQuad();
}

void CDeferredShading::RenderQuad()
{
	glDisable(GL_CULL_FACE);
	glBindVertexArray(m_vertexAttributeObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void CDeferredShading::UpdateLightUniform()
{
	GLint eyePosUnif = glGetUniformLocation(m_LightingPassProgram, "u_eyePos");
	if (eyePosUnif >= 0)
	{
		Vec3 eyePos = CDirector::GetInstance()->GetPerspectiveCamera()->GetCameraPos();
		glUniform3f(eyePosUnif, eyePos.x, eyePos.y, eyePos.z);
	}

	GLint shininessUnif = glGetUniformLocation(m_LightingPassProgram, "u_Material.shininess");
	if (shininessUnif >= 0)
	{
		glUniform1f(shininessUnif, 64);
	}

	const std::vector<CDirectionalLight>& vAllDirectionalLight = CLightManager::GetInstance()->GetAllDirectionalLights();
	for (int i = 0; i < vAllDirectionalLight.size(); ++i)
	{
		CDirectionalLight* pDirLight = const_cast<CDirectionalLight*>(&vAllDirectionalLight[i]);
		std::ostringstream oss;
		oss << "u_AllDirLight[" << i << "]";
		GLint unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".direction").c_str());
		if (unif >= 0)
		{
			if (pDirLight->m_pDebugMesh)
			{
				pDirLight->m_lightDir = -pDirLight->m_pDebugMesh->m_transform.GetPosition();
			}
			glUniform3f(unif, pDirLight->m_lightDir.x, pDirLight->m_lightDir.y, pDirLight->m_lightDir.z);
		}

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".ambient").c_str());
		if (unif >= 0)
			glUniform3f(unif, pDirLight->m_ambientColor.x, pDirLight->m_ambientColor.y, pDirLight->m_ambientColor.z);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".diffuse").c_str());
		if (unif >= 0)
			glUniform3f(unif, pDirLight->m_diffuseColor.x, pDirLight->m_diffuseColor.y, pDirLight->m_diffuseColor.z);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".specular").c_str());
		if (unif >= 0)
			glUniform3f(unif, pDirLight->m_specularColor.x, pDirLight->m_specularColor.y, pDirLight->m_specularColor.z);
	}

	const std::vector<CPointLight>& vAllPointLight = CLightManager::GetInstance()->GetAllPointLights();
	for (int i = 0; i < vAllPointLight.size(); ++i)
	{
		CPointLight* pPointLight = const_cast<CPointLight*>(&vAllPointLight[i]);
		std::ostringstream oss;
		oss << "u_AllPointLight[" << i << "]";
		GLint unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".position").c_str());
		if (unif >= 0)
		{
			if (pPointLight->m_pDebugMesh)
			{
				pPointLight->m_lightPos = pPointLight->m_pDebugMesh->m_transform.GetPosition();
			}
			glUniform3f(unif, pPointLight->m_lightPos.x, pPointLight->m_lightPos.y, pPointLight->m_lightPos.z);
		}

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".ambient").c_str());
		if (unif >= 0)
			glUniform3f(unif, pPointLight->m_ambientColor.x, pPointLight->m_ambientColor.y, pPointLight->m_ambientColor.z);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".diffuse").c_str());
		if (unif >= 0)
			glUniform3f(unif, pPointLight->m_diffuseColor.x, pPointLight->m_diffuseColor.y, pPointLight->m_diffuseColor.z);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".specular").c_str());
		if (unif >= 0)
			glUniform3f(unif, pPointLight->m_specularColor.x, pPointLight->m_specularColor.y, pPointLight->m_specularColor.z);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".constant").c_str());
		if (unif >= 0)
			glUniform1f(unif, pPointLight->m_attenuation_constant);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".linear").c_str());
		if (unif >= 0)
			glUniform1f(unif, pPointLight->m_attenuation_linear);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".quadratic").c_str());
		if (unif >= 0)
			glUniform1f(unif, pPointLight->m_attenuation_quadratic);
	}

	const std::vector<CSpotLight>& vAllSpotLight = CLightManager::GetInstance()->GetAllSpotLights();
	for (int i = 0; i < vAllSpotLight.size(); ++i)
	{
		CSpotLight* pSpotLight = const_cast<CSpotLight*>(&vAllSpotLight[i]);
		std::ostringstream oss;
		oss << "u_AllSpotLight[" << i << "]";
		GLint unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".position").c_str());
		if (unif >= 0)
		{
			if (pSpotLight->m_pDebugMesh)
			{
				pSpotLight->m_lightPos = pSpotLight->m_pDebugMesh->m_transform.GetPosition();
			}
			glUniform3f(unif, pSpotLight->m_lightPos.x, pSpotLight->m_lightPos.y, pSpotLight->m_lightPos.z);
		}

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".direction").c_str());
		if (unif >= 0)
		{
			glUniform3f(unif, pSpotLight->m_lightDir.x, pSpotLight->m_lightDir.y, pSpotLight->m_lightDir.z);
		}

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".ambient").c_str());
		if (unif >= 0)
			glUniform3f(unif, pSpotLight->m_ambientColor.x, pSpotLight->m_ambientColor.y, pSpotLight->m_ambientColor.z);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".diffuse").c_str());
		if (unif >= 0)
			glUniform3f(unif, pSpotLight->m_diffuseColor.x, pSpotLight->m_diffuseColor.y, pSpotLight->m_diffuseColor.z);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".specular").c_str());
		if (unif >= 0)
			glUniform3f(unif, pSpotLight->m_specularColor.x, pSpotLight->m_specularColor.y, pSpotLight->m_specularColor.z);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".constant").c_str());
		if (unif >= 0)
			glUniform1f(unif, pSpotLight->m_attenuation_constant);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".linear").c_str());
		if (unif >= 0)
			glUniform1f(unif, pSpotLight->m_attenuation_linear);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".quadratic").c_str());
		if (unif >= 0)
			glUniform1f(unif, pSpotLight->m_attenuation_quadratic);

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".innerCutoff").c_str());
		if (unif >= 0)
			glUniform1f(unif, DEG_TO_RAD(pSpotLight->fInnerAngle));

		unif = glGetUniformLocation(m_LightingPassProgram, (oss.str() + ".outerCutoff").c_str());
		if (unif >= 0)
			glUniform1f(unif, DEG_TO_RAD(pSpotLight->fOuterAngle));
	}
}
