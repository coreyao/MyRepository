#pragma once

#include "BaseRenderingDataStructure.h"
#include "OpenGL/GLFrameWork.h"

class CMesh : public CBaseMesh
{
public:
	CMesh();
	virtual ~CMesh();

	virtual void Update(float dt);
	virtual void Render();
	void InitFromFile(const char* pMeshFileName);
	void SetTexture(const char* pTextureFileName, int iIndex);
	void SetGLProgram(GLuint theProgram);
	void SetVisible(bool bVisible, const std::string& sSubMeshName);
	void PlayAnim( int iStartFrameIndex, int iEndFrameIndex, bool bLoop = false);

private:
	void InitVBOAndVAO();
	void InitSkeleton();
	void InitUniform();
	void InitMaterial();

private:
	GLuint m_theProgram;

	int m_colorTexUnit;
	GLuint m_Sampler;
	std::vector<Mat4> m_MV;

	std::vector<GLuint> m_vertexDataObj;
	std::vector<GLuint> m_vertexIndexObj;
	std::vector<GLuint> m_vTexture;
	std::vector<GLuint> m_vertexAttributeObj;
};

