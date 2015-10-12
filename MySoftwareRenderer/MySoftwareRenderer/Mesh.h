#pragma once

#include "DataTypes.h"

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
	std::vector<SVertexRuntime> m_vVertexRunTime;
	std::vector<SFaceRuntime> m_vFaceRunTime;
	std::vector<SFaceRuntime> m_vFaceRunTimeOrigin;
};

class CMesh : public CRenderObject
{
public:
	CMesh();
	virtual ~CMesh();

	virtual void Update(float dt);
	virtual void Render();

	void InitFromFile(const char* pMeshFileName);
	void InitFromData(SMeshData* pMeshData);
	void InitRuntimeData();
	void InitShader();

	void SetMaterial(const CMaterial& rMaterial, int iIndex);
	void SetVisible(bool bVisible, const std::string& sSubMeshName);

	SMeshData m_meshData;
	Color4F m_color;
	std::vector<CSubMesh*> m_vSubMesh;
};