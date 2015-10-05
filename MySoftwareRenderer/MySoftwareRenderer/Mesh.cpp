#include "Mesh.h"
#include "Pipeline.h"
#include "Director.h"

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
	for (auto& pSubMesh : m_vSubMesh)
		delete pSubMesh;
}

void CMesh::Update(float dt)
{
	if (m_transform.IsTransformDirty())
	{
		for (auto& rSubmesh : m_vSubMesh)
			m_renderState.m_worldTransform = m_transform.GetTransformMat() * rSubmesh->m_meshMat;
	}
}

void CMesh::Render()
{
	for (int i = 0; i < m_vSubMesh.size(); ++i)
	{
		CSubMesh* rSubmesh = m_vSubMesh[i];
		for (int j = 0; j < rSubmesh->m_vFaceRunTimeOrigin.size(); ++j)
		{
			SFaceRuntime* rFace = &rSubmesh->m_vFaceRunTime[j];
			rFace->m_vertex1 = rSubmesh->m_vFaceRunTimeOrigin[j].m_vertex1;
			rFace->m_vertex2 = rSubmesh->m_vFaceRunTimeOrigin[j].m_vertex2;
			rFace->m_vertex3 = rSubmesh->m_vFaceRunTimeOrigin[j].m_vertex3;
			rFace->m_pRenderState = &m_renderState;
			//rFace->m_fAlpha = 0.7f;
			rFace->m_pRenderState->m_pMaterial = &rSubmesh->m_material;
			CPipeline::GetInstance()->AddFace(rFace);
		}
	}
}

void CMesh::InitFromFile(const char* pMeshFileName)
{
	FILE* pMeshFile = fopen((MESH_FILE_DIR + pMeshFileName).c_str(), "rb");
	if (!pMeshFile)
		return;

	m_meshData.ReadFromFile(pMeshFile);
	InitRuntimeData();
}

void CMesh::InitFromData(SMeshData* pMeshData)
{
	m_meshData = *pMeshData;
	InitRuntimeData();
}

void CMesh::SetMaterial(const CMaterial& rMaterial, int iIndex)
{
	m_vSubMesh[iIndex]->m_material = rMaterial;
}

void CMesh::SetVisible(bool bVisible, const std::string& sSubMeshName)
{
	for (int i = 0; i < m_meshData.m_vSubMesh.size(); ++i)
	{
		if ( m_meshData.m_vSubMesh[i].m_MeshName == sSubMeshName )
		{
			m_vSubMesh[i]->m_bSubMeshVisibility = bVisible;
			break;
		}
	}
}

void CMesh::InitRuntimeData()
{
	for (int i = 0; i < m_meshData.m_vSubMesh.size(); ++i)
	{
		auto& rSubMesh = m_meshData.m_vSubMesh[i];

		CSubMesh* subMesh = new CSubMesh;
		subMesh->m_bSubMeshVisibility = true;
		subMesh->m_meshMat = rSubMesh.m_MeshMatrix;
		subMesh->m_pParent = this;

		for (auto& rVertex : rSubMesh.m_vVertex)
		{
			SVertexRuntime v;
			v.m_pos = Vec4(rVertex.m_pos.x, rVertex.m_pos.y, rVertex.m_pos.z, 1.0f);
			v.m_normal = rVertex.m_normal;
			v.m_tangent = rVertex.m_tangent;
			v.m_UV = rVertex.m_UV;
			v.m_color = rVertex.m_color;
			v.m_boneIndex = v.m_boneIndex;
			v.m_blendWeight = v.m_blendWeight;
			subMesh->m_vVertexRunTime.push_back(v);
		}

		for (auto& rFace : rSubMesh.m_vFace)
		{
			SFaceRuntime curFace;
			curFace.m_vertex1 = subMesh->m_vVertexRunTime[rFace.m_VertexIndex1];
			curFace.m_vertex2 = subMesh->m_vVertexRunTime[rFace.m_VertexIndex2];
			curFace.m_vertex3 = subMesh->m_vVertexRunTime[rFace.m_VertexIndex3];
			curFace.m_pRenderState = &m_renderState;
			curFace.m_bUseNormalizedPos = subMesh->m_bUseNormalizedPos;
			subMesh->m_vFaceRunTimeOrigin.push_back(curFace);
		}

		subMesh->m_vFaceRunTime = subMesh->m_vFaceRunTimeOrigin;

		m_vSubMesh.push_back(subMesh);
	}
}
