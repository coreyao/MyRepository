#include "FrameWork/OpenGL/OpenGLFrameWork.h"
#include "FrameWork/Utility.h"
#include "FrameWork/Image/PNGReader.h"

#include <algorithm>

GLuint theProgram;

const float vertexPositions[] = {
	0.75f, 0.75f, 1.0f, 1.0f,
	0.75f, -0.75f, 1.0f, 1.0f,
	-0.75f, -0.75f, 1.0f, 1.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

GLuint positionBufferObject;
GLuint vao;

const int g_colorTexUnit = 0;
GLuint g_checkerTexture = 0;
GLuint g_sampler = 0;

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

void init()
{
	InitializeProgram();
	InitializeVertexBuffer();
	InitializeTexture();

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
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(12 * sizeof(float)));

	glActiveTexture(GL_TEXTURE0 + g_colorTexUnit);
	glBindTexture(GL_TEXTURE_2D, g_checkerTexture);
	glBindSampler(g_colorTexUnit, g_sampler);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindSampler(g_colorTexUnit, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);

	glutSwapBuffers();
}

void reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}