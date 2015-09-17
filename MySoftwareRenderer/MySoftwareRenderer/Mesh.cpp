#include "Mesh.h"
#include "ApplicationStage.h"
#include "GeometryStage.h"
#include "RasterizationStage.h"
#include "Director.h"

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
}

void CMesh::Update(float dt)
{
	m_transform.m_rotation.x += 30 * dt;
	m_transform.m_rotation.y += 30 * dt;
	m_transform.m_rotation.z += 30 * dt;
}

void CMesh::Render()
{
	for (int i = 0; i < m_meshData.m_vSubMesh.size(); ++i)
	{
		auto& rSubMesh = m_meshData.m_vSubMesh[i];
		for (auto& rFace : rSubMesh.m_vFace)
		{
			vector<SVertexRuntime> vVertex;
			vVertex.push_back(m_vVertexRunTime[i][rFace.m_VertexIndex1]);
			vVertex.push_back(m_vVertexRunTime[i][rFace.m_VertexIndex2]);
			vVertex.push_back(m_vVertexRunTime[i][rFace.m_VertexIndex3]);

			for (auto& rVertex : vVertex)
			{
				Vec4 localPos = Vec4(rVertex.m_pos.x, rVertex.m_pos.y, rVertex.m_pos.z, 1.0f);
				Vec4 worldPos = m_transform.GetTransformMat() * rSubMesh.m_MeshMatrix * localPos;
				rVertex.m_pos = Vec3(worldPos.x, worldPos.y, worldPos.z);
			}

			if (!m_bEnableCullFace || !ApplicationStage::IsBackFace(vVertex[0], vVertex[1], vVertex[2], m_eVertexOrder))
			{
				for (auto& rVertex : vVertex)
					GeometryStage::TransformWorldToScreen(rVertex);

				if (!RasterizationStage::CRasterizer::GetInstance()->IsOutSideScreen(vVertex[0].m_pos.x, vVertex[0].m_pos.y)
					|| !RasterizationStage::CRasterizer::GetInstance()->IsOutSideScreen(vVertex[1].m_pos.x, vVertex[1].m_pos.y)
					|| !RasterizationStage::CRasterizer::GetInstance()->IsOutSideScreen(vVertex[2].m_pos.x, vVertex[2].m_pos.y))
				{
					RasterizationStage::CRasterizer::GetInstance()->DrawAnyTriangle(vVertex[0], vVertex[1], vVertex[2], m_bDrawWireFrame);
				}
			}
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

