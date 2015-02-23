#include "FrameWork/OpenGL/OpenGLFrameWork.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Image/PNGReader.h"
#include "FrameWork/DataStructure.h"

#include <algorithm>

GLuint theProgram;
GLuint perspectiveMatrixUnif;

const float vertexPositions[] = {
	0.75f, 0.75f, 1.0f, 1.0f,
	0.75f, -0.75f, 1.0f, 1.0f,
	-0.75f, -0.75f, 1.0f, 1.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

GLuint positionBufferObject;
GLuint indexBufferObject;
GLuint vao;

const int g_colorTexUnit = 0;
GLuint g_checkerTexture = 0;
GLuint g_sampler = 0;

float perspectiveMatrix[16];
const float fFrustumScale = 1.0f;

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(OpenGLFramework::LoadShader(GL_VERTEX_SHADER, SHADER_FILE_DIR + "Mesh_Vertex_Shader.vert"));
	shaderList.push_back(OpenGLFramework::LoadShader(GL_FRAGMENT_SHADER, SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag"));

	theProgram = OpenGLFramework::CreateProgram(shaderList);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	GLuint colorTextureUnif = glGetUniformLocation(theProgram, "colorTexture");
	glUseProgram(theProgram);
	glUniform1i(colorTextureUnif, g_colorTexUnit);

	perspectiveMatrixUnif = glGetUniformLocation(theProgram, "perspectiveMatrix");
	float fzNear = 1.0f; float fzFar = 1000.0f;
	memset(perspectiveMatrix, 0, sizeof(float) * 16);
	perspectiveMatrix[0] = fFrustumScale;
	perspectiveMatrix[5] = fFrustumScale;
	perspectiveMatrix[10] = (fzFar + fzNear) / (fzNear - fzFar);
	perspectiveMatrix[14] = (2 * fzFar * fzNear) / (fzNear - fzFar);
	perspectiveMatrix[11] = -1.0f;
	glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, perspectiveMatrix);

	glUseProgram(0);
}

void InitializeVertexBuffer()
{
	glGenBuffers(1, &positionBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InitializeTexture()
{
	CPNGReader pngReader(IMAGE_FILE_DIR + "HelloWorld.png");
	if ( pngReader.GetData() )
	{
		glGenTextures(1, &g_checkerTexture);
		glBindTexture(GL_TEXTURE_2D, g_checkerTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pngReader.GetWidth(), pngReader.GetHeight(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, pngReader.GetData());

		glGenSamplers(1, &g_sampler);
		glSamplerParameteri(g_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(g_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glSamplerParameteri(g_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(g_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

CMesh g_mesh;

void InitializeMesh()
{
	FILE* pMeshFile = fopen((LOCAL_FILE_DIR + MESH_FILE_DIR + "Cube.CSTM").c_str(), "rb");
	if ( !pMeshFile )
		return;

	g_mesh.ReadFromFile(pMeshFile);
	for ( auto& rVertex : g_mesh.m_SubMeshVec[0].m_vVectex )
	{
		rVertex.m_position.x /= (rVertex.m_position.z + 100);
		rVertex.m_position.y /= (rVertex.m_position.z + 100);
		rVertex.m_position.z = 1.0f;
	}

	glGenBuffers(1, &positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, g_mesh.m_SubMeshVec[0].m_vVectex.size() * sizeof(SVertex), &g_mesh.m_SubMeshVec[0].m_vVectex.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_mesh.m_SubMeshVec[0].m_vFace.size() * sizeof(SFace), &g_mesh.m_SubMeshVec[0].m_vFace.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void init()
{
	InitializeProgram();
	//InitializeVertexBuffer();
	//InitializeTexture();
	InitializeMesh();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(theProgram);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), 0);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(12 * sizeof(float)));

	//glActiveTexture(GL_TEXTURE0 + g_colorTexUnit);
	//glBindTexture(GL_TEXTURE_2D, g_checkerTexture);
	//glBindSampler(g_colorTexUnit, g_sampler);

	//glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glDrawElements(GL_TRIANGLES, g_mesh.m_SubMeshVec[0].m_vFace.size() * 3, GL_UNSIGNED_INT, 0);

	//glBindSampler(g_colorTexUnit, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	glUseProgram(0);

	glutSwapBuffers();
}

void reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}