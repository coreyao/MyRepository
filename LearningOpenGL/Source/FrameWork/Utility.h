#pragma once

#include "DataTypes.h"

#ifndef SHADER_FILE_DIR
#define SHADER_FILE_DIR std::string("../Resource/Shaders/")
#endif

#ifndef IMAGE_FILE_DIR
#define IMAGE_FILE_DIR std::string("../Resource/Images/")
#endif

#ifndef MESH_FILE_DIR
#define MESH_FILE_DIR std::string("../Resource/Mesh/")
#endif

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

class CBaseMesh
{
public:
	virtual void Render() = 0;
	const SMeshData& GetMeshData() {return m_data;}

	Vec2 m_worldPos;
	Vec3 m_rotation;
	Vec3 m_scale;
	CSkeleton m_skeleton;

protected:
	SMeshData m_data;
};
