#pragma once

#include "DataTypes.h"

class CBaseMesh
{
public:
	CBaseMesh();

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

	STransform m_transform;
	Color4F m_color;

	std::vector<bool> m_vSubMeshVisibility;
	bool m_bEnableCullFace;
	bool m_bDrawWireFrame;
	SMeshData m_meshData;
	EVertexOrder m_eVertexOrder;

protected:
};

class CMesh : public CBaseMesh
{
public:
	CMesh();
	virtual ~CMesh();

	virtual void Update(float dt);
	virtual void Render();

	void InitFromFile(const char* pMeshFileName);
	void SetMaterial(const CMaterial& rMaterial, int iIndex);
	void SetVisible(bool bVisible, const std::string& sSubMeshName);

	std::vector<Mat4> m_MV;
	std::vector<CMaterial> m_vMaterial;
};