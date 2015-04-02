#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <set>

#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"

typedef Matrix4X4 Mat4;

struct Color4F
{
	Color4F() : r(1.0f), g(1.0f), b(1.0f), a(1.0f)
	{
	}

	Color4F( float _r, float _g, float _b, float _a )
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
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
	SVertex()
	{
	}

	Vec3 m_position;
	Vec3 m_normal;
	Vec2 m_texCoord;
	Color4F m_color;
	Vec4 m_boneIndex;
	Vec4 m_blendWeight;
};

struct STextureData
{
	std::string m_sFileName;
};

struct SMaterialData 
{
	std::string m_MaterialName;
	std::vector<STextureData> m_SubTextureVec;
};

struct SBoneKey
{
	SBoneKey()
	{
		memset(m_sBoneName, 0, sizeof(m_sBoneName));
	}

	char m_sBoneName[256];
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
	std::vector<int> m_vSkinBone;
	std::vector<SBoneFrame> m_vFrame;
};

struct SSubMeshData
{
	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	std::string				m_MeshName;
	Mat4					m_MeshMatrix;
	std::vector<SFace>		m_vFace;
	std::vector<SVertex>	m_vVectex;
	SMaterialData			m_cMaterial;
};

struct SMeshData
{
	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	std::vector<SSubMeshData>	m_vSubMesh;
	SSkeletonData				m_skeleton;
};
