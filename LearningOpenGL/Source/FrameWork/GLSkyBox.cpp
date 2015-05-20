#include "GLSkyBox.h"
#include "OpenGL/GLProgramManager.h"
#include "Director.h"

void CGLSkyBox::Init( const std::string& sPositiveX, const std::string& sNegativeX, const std::string& sPositiveY, const std::string& sNegativeY , 
					 const std::string& sPositiveZ, const std::string& sNegativeZ )
{
	 m_CubeMap.Init(sPositiveX, sNegativeX, sPositiveY, sNegativeY, sPositiveZ, sNegativeZ);
	 InitVBOAndVAO();
	 SetGLProgram( CGLProgramManager::GetInstance()->CreateProgramByName("SkyBox") );
	 InitUniform();
}

void CGLSkyBox::Update( float dt )
{
	m_transform.m_pos = CDirector::GetInstance()->GetPerspectiveCamera()->GetCameraPos();
}

void CGLSkyBox::Render()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glUseProgram(m_theProgram);
	glBindVertexArray(m_vertexAttributeObj);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMap.m_cubeTex);
	glBindSampler(0, m_CubeMap.m_cubeSampler);

	GLint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
	if ( modelViewMatrixUnif >= 0 )
	{
		Mat4 viewMatrix = CDirector::GetInstance()->GetPerspectiveCamera()->GetViewMat();
		glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, (viewMatrix * m_transform.GetTransformMat()).m);
	}

	GLint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	if ( perspectiveMatrixUnif >= 0 )
	{
		const Mat4& projMat = CDirector::GetInstance()->GetPerspectiveCamera()->GetProjMat();
		glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, projMat.m);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, (GLsizei)36, GL_UNSIGNED_BYTE, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CGLSkyBox::InitVBOAndVAO()
{
	Vec3 vexBuf[] =
	{
		Vec3(1, -1, 1),  Vec3(1, 1, 1),  Vec3(-1, 1, 1),  Vec3(-1, -1, 1),
		Vec3(1, -1, -1), Vec3(1, 1, -1), Vec3(-1, 1, -1), Vec3(-1, -1, -1)
	};

	glGenBuffers(1, &m_vertexDataObj);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vexBuf), vexBuf, GL_STATIC_DRAW);

	// init index buffer object
	const unsigned char idxBuf[] = {  2, 1, 0, 3, 2, 0, // font
		1, 5, 4, 1, 4, 0, // right
		4, 5, 6, 4, 6, 7, // back
		7, 6, 2, 7, 2, 3, // left
		2, 6, 5, 2, 5, 1, // up
		3, 0, 4, 3, 4, 7  // down
	};

	glGenBuffers(1, &m_vertexIndexObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idxBuf), idxBuf, GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vertexAttributeObj);
	glBindVertexArray(m_vertexAttributeObj);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), 0);
}

void CGLSkyBox::SetGLProgram( GLint theProgram )
{
	m_theProgram = theProgram;
}

void CGLSkyBox::InitUniform()
{
	glUseProgram(m_theProgram);

	GLint colorTextureUnif = glGetUniformLocation(m_theProgram, "u_colorTexture");
	if ( colorTextureUnif >= 0 )
	{
		glUniform1i(colorTextureUnif, 0);
	}

	glUseProgram(0);
}

