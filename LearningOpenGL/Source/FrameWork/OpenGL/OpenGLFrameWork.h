#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <vector>
#include <string>

void init();
void display();
void reshape(int w, int h);

namespace OpenGLFramework
{
	GLuint LoadShader(GLenum eShaderType, const std::string &strShaderFilename);
	GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile);
	GLuint CreateProgram(const std::vector<GLuint> &shaderList);
    std::string FindFileOrThrow(const std::string &strBasename);
}