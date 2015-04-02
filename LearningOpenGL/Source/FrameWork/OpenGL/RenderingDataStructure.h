#pragma once

#include "../Utility.h"
#include "OpenGLFrameWork.h"

class COGLMesh : public CBaseMesh
{
public:
	COGLMesh();
	virtual ~COGLMesh();

	virtual void Render();
	void InitFromFile(const char* pMeshFileName);
	void SetTexture(const char* pTextureFileName, int iIndex);

private:
	void InitSkeleton();
	void InitProgram();
	void InitMaterial();
	void InitVBOAndVAO();

private:
	GLuint m_theProgram;
	int m_colorTexUnit;

	std::vector<GLuint> m_vertexDataObj;
	std::vector<GLuint> m_vertexIndexObj;
	std::vector<GLuint> m_vTexture;
	std::vector<GLuint> m_vertexAttributeObj;

	GLuint m_Sampler;
};

