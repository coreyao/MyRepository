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
	std::vector<CBone> m_vBone; 
};

class CSkeletonAnimator
{
public:
	CSkeletonAnimator();

	void SetTarget(CBaseMesh* pMesh);
	void Update(float fDeltaTime);

protected:
	CBaseMesh* m_pTarget;
	float m_fElapsedTime;
};

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
	GLuint m_Texture;
	GLuint m_Sampler;

	CSkeleton m_skeleton;
};

