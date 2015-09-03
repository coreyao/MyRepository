#pragma once

#include "Utility.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

enum EVertexOrder
{
	EVertexOrder_ClockWise,
	EVertexOrder_Counter_ClockWise,
};

struct Color3B
{
	Color3B() : r(0), g(0), b(0)
	{
	}

	Color3B(int _r, int _g, int _b)
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

struct Color4F
{
	Color4F() : r(1.0f), g(1.0f), b(1.0f), a(1.0f)
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

	Color4F operator*(const Color4F& rh);

	float r;
	float g;
	float b;
	float a;

	static const Color4F WHITE;
	static const Color4F GREEN;
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
	SVertex(){}
	SVertex(const Vec3& pos, const Color4F& color, const Vec2& uv);

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

struct SSubMeshData
{
	std::string				m_MeshName;
	Mat4					m_MeshMatrix;
	std::vector<SFace>		m_vFace;
	std::vector<SVertex>	m_vVertex;
	SMaterialData			m_cMaterial;
};

struct SMeshData
{
	std::vector<SSubMeshData>	m_vSubMesh;
};

struct STransform
{
	STransform()
	{
		m_mat = Mat4::IDENTITY;
		m_scale.set(1.0f, 1.0f, 1.0f);
	}

	Mat4 GetTransformMat();
	Mat4 GetRotationMat();
	void Reset();

	Vec3 m_pos;
	Vec3 m_rotation;
	Vec3 m_scale;

	Mat4 m_mat;
};

class CMaterial
{
public:
	CMaterial();

	//GLuint GetBaseColorTex();
	void SetBaseColorTexture(const std::string& sFileName);

private:
	//GLuint m_baseColorTex;
};
