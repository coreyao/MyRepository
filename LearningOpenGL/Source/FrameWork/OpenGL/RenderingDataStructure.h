#pragma once

#include "../Utility.h"
#include "OpenGLFrameWork.h"

class CBone
{
public:
	CBone() : m_pParent(nullptr)
	{
	}

	void CalcPalette(Vec4* matrixPalette);

	Mat4 m_localMat;
	Mat4 m_worldMat;
	SBoneData m_data;

	std::vector<CBone*> m_vChildren;
	CBone* m_pParent;
};

class CSkeleton
{
public:
	Vec4* GetMatrixPalette();

	Vec4* m_matrixPalette;
	SSkeletonData m_skeletonData;
};

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