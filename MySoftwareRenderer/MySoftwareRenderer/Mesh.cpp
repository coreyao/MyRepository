#include "Mesh.h"
#include "Pipeline.h"
#include "Director.h"

CMesh::CMesh()
: m_bUseNormalizedPos(false)
{
}

CMesh::~CMesh()
{
}

void CMesh::Update(float dt)
{
	if ( m_transform.IsTransformDirty() )
	{
		for (int i = 0; i < m_meshData.m_vSubMesh.size(); ++i)
		{
			auto& rSubMesh = m_meshData.m_vSubMesh[i];
			m_renderState.m_worldTransform = m_transform.GetTransformMat() * rSubMesh.m_MeshMatrix;
		}
	}
}

void CMesh::Render()
{
	for (int i = 0; i < m_meshData.m_vSubMesh.size(); ++i)
	{
		auto& rSubMesh = m_meshData.m_vSubMesh[i];
		for (auto& rFace : rSubMesh.m_vFace)
		{
			SFaceRuntime* curFace = new SFaceRuntime;
			curFace->m_vertex1 = m_vVertexRunTime[i][rFace.m_VertexIndex1];
			curFace->m_vertex2 = m_vVertexRunTime[i][rFace.m_VertexIndex2];
			curFace->m_vertex3 = m_vVertexRunTime[i][rFace.m_VertexIndex3];
			curFace->m_pRenderState = &m_renderState;
			curFace->m_bUseNormalizedPos = m_bUseNormalizedPos;
			CPipeline::GetInstance()->AddFace(curFace);
		}
	}
}

void CMesh::InitFromFile(const char* pMeshFileName)
{
	FILE* pMeshFile = fopen((MESH_FILE_DIR + pMeshFileName).c_str(), "rb");
	if (!pMeshFile)
		return;

	m_meshData.ReadFromFile(pMeshFile);
	m_vSubMeshVisibility.resize(m_meshData.m_vSubMesh.size(), true);
	m_MV.resize(m_meshData.m_vSubMesh.size());
	m_vVertexRunTime.resize(m_meshData.m_vSubMesh.size());
	for (int i = 0; i < m_meshData.m_vSubMesh.size(); ++i)
	{
		auto& rSubMesh = m_meshData.m_vSubMesh[i];
		for (auto& rVertex : rSubMesh.m_vVertex)
		{
			SVertexRuntime v;
			memcpy((void*)&v, (void*)&rVertex, sizeof(SVertex));
			v.m_inverseZ = 0;
			m_vVertexRunTime[i].push_back(v);
		}
	}
}

void CMesh::SetMaterial(const CMaterial& rMaterial, int iIndex)
{
}

void CMesh::SetVisible(bool bVisible, const std::string& sSubMeshName)
{
}

