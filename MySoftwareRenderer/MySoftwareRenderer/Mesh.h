#pragma once

#include "DataTypes.h"

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

class CMesh;
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
		, m_bUseNormalizedPos(false)
		, m_pParent(nullptr)
	{
	}

	CMesh* m_pParent;

	Mat4 m_meshMat;
	CMaterial m_material;
	bool m_bSubMeshVisibility;
	bool m_bUseNormalizedPos;
	std::vector<CVertexRuntime> m_vVertexRunTime;
	std::vector<CFaceRuntime> m_vFaceRunTime;
	std::vector<CFaceRuntime> m_vFaceRunTimeOrigin;
};

class CMesh : public CRenderObject
{
public:
	CMesh();
	virtual ~CMesh();

	virtual void Update(float dt);
	virtual void Render();

	void InitFromFile(const char* pMeshFileName, bool bSkinMesh);
	void InitFromData(SMeshData* pMeshData);
	void InitRuntimeData();
	void InitShader();
	void InitSkeleton();

	void SetMaterial(const CMaterial& rMaterial, int iIndex);
	void SetVisible(bool bVisible, const std::string& sSubMeshName);

	SMeshData m_meshData;
	Color4F m_color;
	std::vector<CSubMesh*> m_vSubMesh;

	CSkeleton m_skeleton;
	CSkeletonAnimator m_animator;

	bool m_bSkinMesh;
};