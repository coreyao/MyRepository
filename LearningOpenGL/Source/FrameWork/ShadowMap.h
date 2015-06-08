#pragma once

#include "OpenGL/GLFrameWork.h"
#include "Utility.h"

class CDirectionalLight;
class CShadowmap
{
public:
	void Init( CDirectionalLight* pDirLight );
	void PreRender();
	void PostRender();

	GLuint GetDepthMapTex();
	void DebugRenderShadowMap();

private:
	GLuint m_depthMapFBO;
	GLuint m_depthMapTex;

	GLuint m_theProgram;
	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
	GLuint m_vertexAttributeObj;

	Mat4 m_lightViewMat;
	Mat4 m_lightProjMat;
};