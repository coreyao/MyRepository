#pragma once

#include "../DataTypes.h"
#include "OpenGLFrameWork.h"

class COGLMesh : public CBaseMesh
{
public:
	COGLMesh();
	virtual ~COGLMesh();

	virtual void Render();
	void InitFromFile(const char* pMeshFileName);
	void SetTexture(const char* pTextureFileName);

	GLuint m_theProgram;
	int m_colorTexUnit;

	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
	GLuint m_vertexAttributeObj;
	GLuint m_Texture;
	GLuint m_Sampler;
};