#pragma once

#include "BaseRenderingDataStructure.h"
#include "OpenGL/GLFrameWork.h"

class CMesh;
class CMeshSocket
{
public:
	CMeshSocket(const CMesh* pTarget, const std::string& sBoneName, const STransform& offset);
	Mat4 GetWorldMat();

private:
	std::string m_sBoneName;
	STransform m_offset;
	const CMesh* m_pTarget;
};

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
	void PlayAnim( int iStartFrameIndex, int iEndFrameIndex, bool bLoop, std::function<void(void)> callback);

	std::vector<CMeshSocket> m_vSocket;

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

