#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#define RESOLUTION_WIDTH 800
#define RESOLUTION_HEIGHT 600

void init();
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse_down( int button, int state, int x, int y );
void mouse_move( int x,int y );
void mouse_scroll( int x,int y );

namespace OpenGLFramework
{
	GLuint LoadShader(GLenum eShaderType, const std::string &strShaderFilename);
	GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile);
	GLuint CreateProgram(const std::vector<GLuint> &shaderList);
    std::string FindFileOrThrow(const std::string &strBasename);
}