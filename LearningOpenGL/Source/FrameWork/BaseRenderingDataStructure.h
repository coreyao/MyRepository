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

protected:
	CBaseMesh* m_pTarget;
	float m_fElapsedTime;
	bool m_bLoop;
};

class CBaseMesh
{
public:
	CBaseMesh();

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
	const SMeshData& GetMeshData() {return m_data;}

	Vec3 m_worldPos;
	Vec3 m_rotation;
	Vec3 m_scale;
	CSkeleton m_skeleton;

	bool m_bEnableCullFace;

protected:
	SMeshData m_data;
};