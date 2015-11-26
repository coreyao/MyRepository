#pragma once

#include "OpenGL/GLFrameWork.h"
#include "Utility.h"

class CBloom
{
public:
	void Init();
	void PreRender();
	void PostRender();

	void RenderQuad();

	GLuint m_firstPassProgram;
	GLuint m_firstPassLighterProgram;
	GLuint m_blurProgram;
	GLuint m_finalProgram;
	float m_fExposure;

private:
	GLuint m_BloomFBO;
	GLuint m_PingpongFBO[2];

	GLuint m_HDRTex;
	GLuint m_BloomTex;
	GLuint m_PingpongBlurTex[2];

	GLuint m_Sampler;
	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
	GLuint m_vertexAttributeObj;
};