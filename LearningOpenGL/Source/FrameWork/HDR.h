#pragma once

#include "OpenGL/GLFrameWork.h"
#include "Utility.h"

class CHDR
{
public:
	void Init();
	void PreRender();
	void PostRender();

	GLuint GetHDRTex();

	GLuint m_theProgram;

private:
	GLuint m_HDRFBO;

	float m_fExposure;

	GLuint m_Sampler;
	GLuint m_HDRTex;
	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
	GLuint m_vertexAttributeObj;
};