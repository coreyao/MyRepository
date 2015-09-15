#pragma once

#include "DataTypes.h"

class CMesh : public CRenderObject
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

	STransform m_transform;
	Color4F m_color;

	std::vector<bool> m_vSubMeshVisibility;
	SMeshData m_meshData;
};