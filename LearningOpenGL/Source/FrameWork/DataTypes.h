#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <stack>

#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"

struct Color3B
{
	Color3B() : r(0), g(0), b(0)
	{
	}

	Color3B( int _r, int _g, int _b )
	{
		r = _r;
		g = _g;
		b = _b;
	}

	Color3B operator+(const Color3B& rh);
	Color3B operator-(const Color3B& rh);
	Color3B operator*(float fScale);
	Color3B operator/(float fScale);
	Color3B operator*=(float fScale);
	Color3B operator*(const Color3B& rh);
	Color3B operator*=(const Color3B& rh);

	int r;
	int g;
	int b;

	static const Color3B WHITE;
	static const Color3B GREEN;
};

class Color4F
{
public:
	Color4F() : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
	{
	}

	Color4F(float _r, float _g, float _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	Color4F(Color3B color3B, float _a)
	{
		r = color3B.r / 255.0f;
		g = color3B.g / 255.0f;
		b = color3B.b / 255.0f;
		a = _a;
	}

	Color4F operator*(const Color4F& rh) const;
	Color4F operator-(const Color4F& rh) const;
	Color4F operator-(float fScalar) const;
	Color4F operator/(float fScalar) const;
	Color4F operator*(float fScalar) const;
	void operator+=(const Color4F& rh);
	Color4F operator+(const Color4F& rh) const;

	void operator*=(float fScalar);

	void Set(float _r, float _g, float _b, float _a);

	float r;
	float g;
	float b;
	float a;

	static const Color4F WHITE;
	static const Color4F GREEN;
	static const Color4F BLACK;
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

struct SSkinMeshVertex
{
	Vec3 m_position;
	Vec3 m_normal;
	Vec3 m_tangent;
	Vec2 m_texCoord;
	Color4F m_color;
	Vec4 m_boneIndex;
	Vec4 m_blendWeight;
};

struct SCommonVertex
{
	SCommonVertex(){}
	SCommonVertex(const Vec3& pos, const Color4F& color, const Vec2& uv);

	Vec3 m_pos;
	Color4F m_color;
	Vec2 m_UV;
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
	std::vector<SSkinMeshVertex>	m_vVectex;
	SMaterialData			m_cMaterial;
};

struct SMeshData
{
	void WriteToFile(FILE* pFile);
	void ReadFromFile(FILE* pFile);

	std::vector<SSubMeshData>	m_vSubMesh;
	SSkeletonData				m_skeleton;
};

struct STransform
{
public:
	STransform()
	{
		m_mat = Mat4::IDENTITY;
		m_scale.set(1.0f, 1.0f, 1.0f);
		m_bTransformDirty = true;
		m_bUseQuaternion = false;
	}

	Mat4 GetTransformMat();
	Mat4 GetRotationMat();
	void Reset();

	const Vec3& GetPosition() const;
	void SetPosition(const Vec3& pos);
	void SetPositionX(float val);
	void SetPositionY(float val);
	void SetPositionZ(float val);

	const Vec3& GetRotation() const;
	void SetRotation(const Vec3& rot);
	void SetRotation(const Quaternion& rot);

	const Vec3& GetScale() const;
	void SetScale(const Vec3& scale);

	bool IsTransformDirty();
	void SetUseQuaternion(bool bVal);

private:
	Vec3 m_pos;
	Vec3 m_rotation;
	Quaternion m_quat;
	Vec3 m_scale;

	Mat4 m_mat;

	bool m_bTransformDirty;
	bool m_bUseQuaternion;
};
