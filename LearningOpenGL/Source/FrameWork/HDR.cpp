#include "HDR.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"
#include "Light.h"
#include "Mesh.h"

void CHDR::Init()
{
	// - Create floating point color buffer
	glGenTextures(1, &m_HDRTex);
	glBindTexture(GL_TEXTURE_2D, m_HDRTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// - Create depth buffer
	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);

	glGenFramebuffers(1, &m_HDRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_HDRFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_HDRTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		int i = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	SCommonVertex vVertex[4] = {
		SCommonVertex(Vec3(-1, -1, 0), Color4F(1, 1, 1, 1), Vec2(0, 1)),
		SCommonVertex(Vec3(-1, 1, 0), Color4F(1, 1, 1, 1), Vec2(0, 0)),
		SCommonVertex(Vec3(1, 1, 0), Color4F(1, 1, 1, 1), Vec2(1, 0)),
		SCommonVertex(Vec3(1, -1, 0), Color4F(1, 1, 1, 1), Vec2(1, 1))
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
}

void CHDR::PreRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_HDRTex);
}

void CHDR::PostRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_colorTexture");
	if (colorTextureUnif >= 0)
	{
		glUniform1i(colorTextureUnif, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_HDRTex);
	}

	glDisable(GL_CULL_FACE);
	glUseProgram(m_theProgram);
	glBindVertexArray(m_vertexAttributeObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

GLuint CHDR::GetHDRTex()
{
	return m_HDRTex;
}

