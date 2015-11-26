#include "HDR.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"
#include "Light.h"
#include "Mesh.h"

void CHDR::Init()
{
	glGenFramebuffers(1, &m_HDRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_HDRFBO);
	glGenTextures(1, &m_HDRTex);
	glBindTexture(GL_TEXTURE_2D, m_HDRTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, 0);
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_HDRTex, 0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	glGenBuffers(1, &m_vertexDataObj);
	glGenBuffers(1, &m_vertexIndexObj);
	glGenVertexArrays(1, &m_vertexAttributeObj);

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(SCommonVertex), vVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short), vVertexIndex, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(m_vertexAttributeObj);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_pos));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SCommonVertex), (GLvoid*) offsetof(SCommonVertex, m_UV));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_theProgram = CGLProgramManager::GetInstance()->CreateProgramByName("HDR");
	m_fExposure = 1.0f;
}

void CHDR::PreRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_HDRFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CHDR::PostRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_theProgram);

	GLint unif = glGetUniformLocation(m_theProgram, "u_exposure");
	if (unif >= 0)
		glUniform1f(unif, 1.0f);

	GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_colorTexture");
	if (colorTextureUnif >= 0)
	{
		glUniform1i(colorTextureUnif, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_HDRTex);
		glBindSampler(0, m_Sampler);
	}

	glDisable(GL_CULL_FACE);
	glBindVertexArray(m_vertexAttributeObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

GLuint CHDR::GetHDRTex()
{
	return m_HDRTex;
}

