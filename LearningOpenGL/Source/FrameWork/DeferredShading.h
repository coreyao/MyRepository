#pragma once

#include "OpenGL/GLFrameWork.h"
#include "Utility.h"

class CDeferredShading
{
public:
	void Init();
	void PreRender();
	void PostRender();
	void RenderQuad();
	void UpdateLightUniform();

	GLuint m_GeometryPassProgram;
	GLuint m_LightingPassProgram;

private:
	GLuint m_GBuffer;
	GLuint m_PositionTex;
	GLuint m_NormalTex;
	GLuint m_DiffuseTex;

	GLuint m_Sampler;
	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
	GLuint m_vertexAttributeObj;
};