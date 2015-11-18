#include "ShadowMap.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"
#include "Light.h"
#include "Mesh.h"

const int SHADOW_WIDTH = 2048;
const int SHADOW_HEIGHT = 2048;

void CShadowmap::Init(CDirectionalLight* pDirLight)
{
	glGenFramebuffers(1, &m_depthMapFBO);

	glGenTextures(1, &m_depthMapTex);
	glBindTexture(GL_TEXTURE_2D, m_depthMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &m_vertexDataObj);
	glGenBuffers(1, &m_vertexIndexObj);
	glGenVertexArrays(1, &m_vertexAttributeObj);

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

	m_lightProjMat = Mat4::CreateOrthegraphicsMat(-SHADOW_WIDTH / 2, SHADOW_WIDTH / 2, SHADOW_HEIGHT / 2, -SHADOW_HEIGHT / 2, 1.0f, 3000.0f);
	m_lightViewMat = Mat4::CreateLookAt(pDirLight->m_lightPos, -pDirLight->m_lightDir, Vec3(0, 0, 1));

	m_theDebugProgram = CGLProgramManager::GetInstance()->CreateProgramByName("DebugShadowMap");
	m_theProgram = CGLProgramManager::GetInstance()->CreateProgramByName("ShadowMapping");
}

void CShadowmap::PreRender()
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CDirector::GetInstance()->m_bDrawShadowMap = true;
}

void CShadowmap::PostRender()
{
	CDirector::GetInstance()->m_bDrawShadowMap = false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLuint CShadowmap::GetDepthMapTex()
{
	return m_depthMapTex;
}

void CShadowmap::DebugRenderShadowMap()
{
	GLint colorTextureUnif = glGetUniformLocation(m_theDebugProgram, "u_colorTexture");
	if ( colorTextureUnif >= 0 )
	{
		glUniform1i(colorTextureUnif, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_depthMapTex);
	}
	
	glDisable(GL_CULL_FACE);
	glUseProgram(m_theDebugProgram);
	glBindVertexArray(m_vertexAttributeObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

