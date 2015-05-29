#pragma once

#include "Utility.h"

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

class CBaseMesh;
class CSkeletonAnimator
{
public:
	CSkeletonAnimator();

	void SetTarget(CBaseMesh* pMesh);
	void Update(float fDeltaTime);
	void Reset();
	void PlayAnim(int iStartFrameIndex, int iEndFrameIndex, bool bLoop, std::function<void(void)> callback);

protected:
	CBaseMesh* m_pTarget;
	float m_fElapsedTime;
	bool m_bLoop;

	int m_iStartFrameIndex;
	int m_iEndFrameIndex;

	std::function<void(void)> m_callback;
	float m_fBlendElapsedTime;
};

class CBaseMesh
{
public:
	CBaseMesh();

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
	const SMeshData& GetMeshData() {return m_data;}

	STransform m_transform;
	Color4F m_color;
	CSkeleton m_skeleton;
	CSkeletonAnimator m_animator;

	std::vector<bool> m_vSubMeshVisibility;
	bool m_bEnableCullFace;
	bool m_bDrawWireFrame;
	SMeshData m_data;

protected:
};