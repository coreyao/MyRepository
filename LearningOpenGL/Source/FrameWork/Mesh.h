#pragma once

#include "Light.h"
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

class CBone
{
public:
	CBone() : m_pParent(nullptr)
	{
	}

	Mat4 UpdateWorldMat();
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
	CSkeleton() : m_pMatrixPalette(nullptr) {}

	Vec4* GetMatrixPalette();

	Vec4* m_pMatrixPalette;
	std::vector<CBone> m_vBone;
	std::vector<CBone*> m_vSkinBone;
};

class CSkeletonAnimator
{
public:
	CSkeletonAnimator();

	void SetTarget(CMesh* pMesh);
	void Update(float fDeltaTime);
	void Reset();
	void PlayAnim(int iStartFrameIndex, int iEndFrameIndex, bool bLoop, std::function<void(void)> callback);

protected:
	CMesh* m_pTarget;
	float m_fElapsedTime;
	bool m_bLoop;

	int m_iStartFrameIndex;
	int m_iEndFrameIndex;

	std::function<void(void)> m_callback;
	float m_fBlendElapsedTime;
};

class CMesh;
class CSubMesh
{
public:
	CSubMesh()
		: m_bSubMeshVisibility(true)
		, m_pParent(nullptr)
		, m_iIndex(-1)
	{
	}

	CMesh* m_pParent;
	int m_iIndex;

	Mat4 m_meshMat;
	CMaterial m_material;
	bool m_bSubMeshVisibility;

	GLuint m_vertexDataObj;
	GLuint m_vertexIndexObj;
	GLuint m_vertexAttributeObj;
};

class CMesh : public CObject
{
public:
	CMesh();
	virtual ~CMesh();

	virtual void Update(float dt);
	virtual void Render();

	void InitFromFile(const char* pMeshFileName);
	void SetMaterial(const CMaterial& rMaterial, int iIndex);
	void SetGLProgram(GLuint theProgram);
	void SetVisible(bool bVisible, const std::string& sSubMeshName);
	void SetLightEnable(bool bEnable);
	void PlayAnim( int iStartFrameIndex, int iEndFrameIndex, bool bLoop, std::function<void(void)> callback);

	GLuint m_Sampler;

	SMeshData m_meshData;
	Color4F m_color;
	bool m_bSkinMesh;
	bool m_bFlipNormal;

	CSkeleton m_skeleton;
	CSkeletonAnimator m_animator;
	std::vector<CSubMesh*> m_vSubMesh;
	std::vector<CMeshSocket> m_vSocket;

private:
	void InitSubMesh();
	void InitSkeleton();
	void UpdateLightUniform();
	void UpdateMaterialUniform(CSubMesh* pSubMesh);
};

