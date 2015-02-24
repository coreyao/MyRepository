#pragma once

#include <cstdio>
#include <string>
#include <vector>

struct Vec2
{
	Vec2() : x(0.0f), y(0.0f)
	{
	}

	float x;
	float y;
};

struct Vec3
{
	Vec3() : x(0.0f), y(0.0f), z(0.0f)
	{
	}

	float x;
	float y;
	float z;
};

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

class CMesh
{
public:
	virtual void Render() = 0;

private:
	SMeshData m_data;
};