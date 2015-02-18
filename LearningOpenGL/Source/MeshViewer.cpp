#include "FrameWork/OpenGL/OpenGLFrameWork.h"
#include "FrameWork/Utility.h"

#include <algorithm>

const std::string strVertexShader(
	"#version 330\n"
	"layout(location = 0) in vec4 position;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = position;\n"
	"}\n"
	);

const std::string strFragmentShader(
	"#version 330\n"
	"out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
	"}\n"
	);

GLuint theProgram;

const float vertexPositions[] = {
	0.75f, 0.75f, 1.0f, 1.0f,
	0.75f, -0.75f, 1.0f, 1.0f,
	-0.75f, -0.75f, 1.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint vao;

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(OpenGLFramework::CreateShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(OpenGLFramework::CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = OpenGLFramework::CreateProgram(shaderList);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void InitializeVertexBuffer()
{
	glGenBuffers(1, &positionBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init()
{
	InitializeProgram();
	InitializeVertexBuffer();

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

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	glutSwapBuffers();
}

void reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}