#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <set>

#include "cml/cml.h"

typedef cml::matrix< float, cml::fixed<3,3>, cml::col_basis, cml::col_major> Mat3;
typedef cml::matrix< float, cml::fixed<4,4>, cml::col_basis, cml::col_major> Mat4;

typedef cml::vector< float, cml::fixed<2> > Vec2;
typedef cml::vector< float, cml::fixed<3> > Vec3;
typedef cml::vector< float, cml::fixed<4> > Vec4;

typedef cml::quaternion< float, cml::fixed<>, cml::scalar_first> Quaternion;

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

struct SBoneKey
{
	Vec3 m_translation;
	Quaternion m_rotation;
	Vec3 m_scale;
};

struct SBoneFrame 
{
	SBoneFrame()
		: m_iIndex(-1)
		, m_fTime(0.0f)
	{
	}

	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	int m_iIndex;
	float m_fTime;
	std::vector<SBoneKey> m_vKey;
};

struct SBoneData
{
	SBoneData()
		: m_iIndex(-1)
		, m_iParentIndex(-1)
	{
	}

	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	int m_iParentIndex;
	int m_iIndex;
	std::string  m_sName;
	Mat4 m_originalBindMat;
	Mat4 m_inverseBindMat;
	std::vector<int> m_vChildIndex;
};

class SSkeletonData
{
public:
	std::vector<SBoneData> m_vBone;
	std::set<int> m_vSkinBone;
	std::vector<SBoneFrame> m_vFrame;
};

struct SMeshData
{
	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	std::string				m_MeshName;
	Mat4					m_MeshMatrix;
	std::vector<SFace>		m_vFace;
	std::vector<SVertex>	m_vVectex;
	SMaterialData			m_cMaterial;
	SSkeletonData			m_skeleton;
};
