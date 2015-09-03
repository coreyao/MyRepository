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
	for (auto& rSubMesh : m_meshData.m_vSubMesh)
	{
		for (auto& rFace : rSubMesh.m_vFace)
		{
			vector<SVertex> vVertex;
			vVertex.push_back(rSubMesh.m_vVertex[rFace.m_VertexIndex1]);
			vVertex.push_back(rSubMesh.m_vVertex[rFace.m_VertexIndex2]);
			vVertex.push_back(rSubMesh.m_vVertex[rFace.m_VertexIndex3]);

			for (auto& rVertex : vVertex)
			{
				Vec4 localPos = Vec4(rVertex.m_pos.x, rVertex.m_pos.y, rVertex.m_pos.z, 1.0f);
				Vec4 worldPos = m_transform.GetTransformMat() * rSubMesh.m_MeshMatrix * localPos;
				rVertex.m_pos = Vec3(worldPos.x, worldPos.y, worldPos.z);
			}

			if (!ApplicationStage::IsBackFace(vVertex[0], vVertex[1], vVertex[2], m_eVertexOrder))
			{
				for (auto& rVertex : vVertex)
					GeometryStage::TransformWorldToScreen(rVertex);

				if (!RasterizationStage::IsOutSideScreen(vVertex[0].m_pos.x, vVertex[0].m_pos.y)
					|| !RasterizationStage::IsOutSideScreen(vVertex[1].m_pos.x, vVertex[1].m_pos.y)
					|| !RasterizationStage::IsOutSideScreen(vVertex[2].m_pos.x, vVertex[2].m_pos.y))
				{
					RasterizationStage::DrawTriangle(vVertex[0], vVertex[1], vVertex[2]);
				}
			}
		}
	}
}

void CMesh::InitFromFile(const char* pMeshFileName)
{
}

void CMesh::SetMaterial(const CMaterial& rMaterial, int iIndex)
{
}

void CMesh::SetVisible(bool bVisible, const std::string& sSubMeshName)
{
}

CBaseMesh::CBaseMesh()
: m_eVertexOrder(EVertexOrder_ClockWise)
{
}
