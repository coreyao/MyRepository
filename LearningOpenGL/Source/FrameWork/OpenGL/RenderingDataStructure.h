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
	void SetTexture(const char* pTextureFileName);

private:
	void InitSkeleton();
	void InitProgram();
	void InitMaterial();
	void InitVBOAndVAO();

private:
	GLuint m_theProgram;
	int m_colorTexUnit;

	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
	GLuint m_vertexAttributeObj;
	vector<GLuint> m_vTexture;
	GLuint m_Sampler;
};

