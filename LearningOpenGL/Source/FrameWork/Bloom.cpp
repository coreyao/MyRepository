#include "Bloom.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"
#include "Light.h"
#include "Mesh.h"

void CBloom::Init()
{
	glGenFramebuffers(1, &m_BloomFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_BloomFBO);
	glGenTextures(1, &m_HDRTex);
	glBindTexture(GL_TEXTURE_2D, m_HDRTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, 0);
	glGenTextures(1, &m_BloomTex);
	glBindTexture(GL_TEXTURE_2D, m_BloomTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, 0);
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_HDRTex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_BloomTex, 0);
	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, DrawBuffers);

	glGenFramebuffers(2, m_PingpongFBO);
	glGenTextures(2, m_PingpongBlurTex);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_PingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, m_PingpongBlurTex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PingpongBlurTex[i], 0);
	}

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	m_firstPassProgram = CGLProgramManager::GetInstance()->CreateProgramByName("BloomFirstPass");
	m_firstPassLighterProgram = CGLProgramManager::GetInstance()->CreateProgramByName("BloomFirstPass_Lighter");
	m_blurProgram = CGLProgramManager::GetInstance()->CreateProgramByName("Blur");
	m_finalProgram = CGLProgramManager::GetInstance()->CreateProgramByName("BloomFinal");
	m_fExposure = 1.0f;
}

void CBloom::PreRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_BloomFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CBloom::PostRender()
{
	GLboolean horizontal = true, first_iteration = true;
	GLuint amount = 10;
	for (GLuint i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_PingpongFBO[horizontal]);
		glUseProgram(m_blurProgram);
		glUniform1i(glGetUniformLocation(m_blurProgram, "horizontal"), horizontal);
		GLint colorTextureUnif = glGetUniformLocation(m_blurProgram, "image");
		if (colorTextureUnif >= 0)
		{
			glUniform1i(colorTextureUnif, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? m_BloomTex : m_PingpongBlurTex[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
			glBindSampler(0, m_Sampler);
		}
		RenderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(m_finalProgram);
	GLint unif = glGetUniformLocation(m_finalProgram, "exposure");
	if (unif >= 0)
		glUniform1f(unif, m_fExposure);

	GLint colorTextureUnif = glGetUniformLocation(m_finalProgram, "bloomBlur");
	if (colorTextureUnif >= 0)
	{
		glUniform1i(colorTextureUnif, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? m_BloomTex : m_PingpongBlurTex[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		glBindSampler(0, m_Sampler);
	}

	colorTextureUnif = glGetUniformLocation(m_finalProgram, "scene");
	if (colorTextureUnif >= 0)
	{
		glUniform1i(colorTextureUnif, 1);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, m_HDRTex);
		glBindSampler(1, m_Sampler);
	}

	RenderQuad();
}

void CBloom::RenderQuad()
{
	glDisable(GL_CULL_FACE);
	glBindVertexArray(m_vertexAttributeObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}
