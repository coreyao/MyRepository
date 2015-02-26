#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "cml/cml.h"

typedef cml::matrix< float, cml::fixed<3,3>, cml::col_basis, cml::col_major> Mat3;
typedef cml::matrix< float, cml::fixed<4,4>, cml::col_basis, cml::col_major> Mat4;

typedef cml::vector< float, cml::fixed<2> > Vec2;
typedef cml::vector< float, cml::fixed<3> > Vec3;
typedef cml::vector< float, cml::fixed<4> > Vec4;

struct Color4F
{
	Color4F() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
	{
	}

	float r;
	float g;
	float b;
	float a;
};

struct SFace
{
	SFace()
		: m_VertexIndex1(0)
		, m_VertexIndex2(0)
		, m_VertexIndex3(0)
	{
	}

	int m_VertexIndex1;
	int m_VertexIndex2;
	int m_VertexIndex3;
};

struct SVertex
{
	Vec3 m_position;
	Vec3 m_normal;
	Vec2 m_texCoord;
	Color4F m_color;
	Vec4 m_boneIndex;
	Vec4 m_blendWeight;
};

struct STextureData
{
	STextureData()
	{
	}

	std::string m_sFileName;
};

struct SMaterialData 
{
	SMaterialData()
	{
	}

	std::string m_MaterialName;
	std::vector<STextureData> m_SubTextureVec;
};

struct SSubMeshData
{
	SSubMeshData()
	{
		memset(m_SubMeshMatrix, 0, sizeof(m_SubMeshMatrix));
	}

	std::string m_SubMeshName;
	float  m_SubMeshMatrix[16];
	std::vector<SFace>	m_vFace;
	std::vector<SVertex> m_vVectex;
	SMaterialData m_cMaterial;
};

struct SMeshData
{
	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	std::string m_MeshName;
	std::vector<SSubMeshData> m_SubMeshVec;
};

struct SBoneData
{
	SBoneData()
		: m_iIndex(-1)
		, m_iParentIndex(-1)
	{
	}

	Mat4 m_inverseBindMat;

	int m_iIndex;
	int m_iParentIndex;
	std::vector<int> m_vChildIndex;
};

class SSkeletonData
{
public:
	std::vector<SBoneData> m_vBone;
};

