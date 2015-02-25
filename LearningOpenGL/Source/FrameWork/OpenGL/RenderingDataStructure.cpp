#include "RenderingDataStructure.h"
#include "../Utility.h"
#include "../Image/PNGReader.h"
#include <algorithm>

COGLMesh::COGLMesh()
	: m_vertexDataObj(0)
	, m_vertexIndexObj(0)
	, m_vertexAttributeObj(0)
	, m_Texture(0)
	, m_Sampler(0)
	, m_theProgram(0)
	, m_colorTexUnit(0)
{
}

COGLMesh::~COGLMesh()
{
}

void COGLMesh::InitFromFile( const char* pMeshFileName )
{
	FILE* pMeshFile = fopen((MESH_FILE_DIR + pMeshFileName).c_str(), "rb");
	if ( !pMeshFile )
		return;

	m_data.ReadFromFile(pMeshFile);

	if ( m_data.m_SubMeshVec.empty() )
		return;

	std::vector<GLuint> shaderList;
	shaderList.push_back(OpenGLFramework::LoadShader(GL_VERTEX_SHADER, SHADER_FILE_DIR + "Mesh_Vertex_Shader.vert"));
	shaderList.push_back(OpenGLFramework::LoadShader(GL_FRAGMENT_SHADER, SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag"));
	m_theProgram = OpenGLFramework::CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	GLuint colorTextureUnif = glGetUniformLocation(m_theProgram, "colorTexture");
	glUseProgram(m_theProgram);
	glUniform1i(colorTextureUnif, m_colorTexUnit);

	GLuint perspectiveMatrixUnif = glGetUniformLocation(m_theProgram, "perspectiveMatrix");
	Mat4 perspectiveMatrix;
	cml::matrix_perspective_xfov_RH(perspectiveMatrix, 90.0f, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 1000.0f, cml::z_clip_neg_one);
	glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, perspectiveMatrix.data());

	glUseProgram(0);

	glGenBuffers(1, &m_vertexDataObj);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glBufferData(GL_ARRAY_BUFFER, m_data.m_SubMeshVec[0].m_vVectex.size() * sizeof(SVertex), &m_data.m_SubMeshVec[0].m_vVectex.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_vertexIndexObj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_data.m_SubMeshVec[0].m_vFace.size() * sizeof(SFace), &m_data.m_SubMeshVec[0].m_vFace.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::string sTextureFile;
	if ( !m_data.m_SubMeshVec[0].m_cMaterial.m_SubTextureVec.empty() )
		sTextureFile = m_data.m_SubMeshVec[0].m_cMaterial.m_SubTextureVec[0].m_sFileName;

	if ( !sTextureFile.empty() )
	{
		CPNGReader pngReader(sTextureFile);
		if ( pngReader.GetData() )
		{
			glGenTextures(1, &m_Texture);
			glBindTexture(GL_TEXTURE_2D, m_Texture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pngReader.GetWidth(), pngReader.GetHeight(), 0,
				GL_RGBA, GL_UNSIGNED_BYTE, pngReader.GetData());

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glGenVertexArrays(1, &m_vertexAttributeObj);
	glBindVertexArray(m_vertexAttributeObj);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexDataObj);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)(6 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void COGLMesh::Render()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	glUseProgram(m_theProgram);
	glBindVertexArray(m_vertexAttributeObj);

	if ( m_Texture > 0 )
	{
		glActiveTexture(GL_TEXTURE0 + m_colorTexUnit);
		glBindTexture(GL_TEXTURE_2D, m_Texture);
		glBindSampler(m_colorTexUnit, m_Sampler);
	}

	GLuint modelViewMatrixUnif = glGetUniformLocation(m_theProgram, "modelViewMatrix");
	Mat4 viewMatrix;
	cml::matrix_translation(viewMatrix, Vec3(0.0f, 0.0f, -100.0f));

	Mat4 RotationMatrix;
	cml::matrix_rotation_euler(RotationMatrix, m_rotation[0], m_rotation[1], m_rotation[2], cml::euler_order_xyz);

	Mat4 TranslationMatrix;
	cml::matrix_translation(TranslationMatrix, m_worldPos);

	Mat4 ModelViewMatrix;
	ModelViewMatrix = viewMatrix * TranslationMatrix * RotationMatrix;

	glUniformMatrix4fv(modelViewMatrixUnif, 1, GL_FALSE, ModelViewMatrix.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexIndexObj);
	glDrawElements(GL_TRIANGLES, m_data.m_SubMeshVec[0].m_vFace.size() * 3, GL_UNSIGNED_INT, 0);

	glBindSampler(m_colorTexUnit, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void COGLMesh::SetTexture( const char* pTextureFileName )
{
	CPNGReader pngReader(pTextureFileName);
	if ( pngReader.GetData() )
	{
		glGenTextures(1, &m_Texture);
		glBindTexture(GL_TEXTURE_2D, m_Texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pngReader.GetWidth(), pngReader.GetHeight(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, pngReader.GetData());

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

