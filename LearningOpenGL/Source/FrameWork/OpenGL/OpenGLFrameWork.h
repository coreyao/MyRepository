#pragma once

#include "GL/freeglut.h"
#include <vector>
#include <string>

namespace OpenGLFramework
{
	GLuint LoadShader(GLenum eShaderType, const std::string &strShaderFilename);
	GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile, const std::string &strShaderName);
	GLuint CreateProgram(const std::vector<GLuint> &shaderList);
	std::string FindFileOrThrow(const std::string &strBasename);
}