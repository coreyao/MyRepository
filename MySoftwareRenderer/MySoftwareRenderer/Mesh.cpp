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
	/*m_transform.m_rotation.x += 30 * dt;
	m_transform.m_rotation.y += 30 * dt;
	m_transform.m_rotation.z += 30 * dt;*/
}

void CMesh::Render()
{
	for (int i = 0; i < m_meshData.m_vSubMesh.size(); ++i)
	{
		auto& rSubMesh = m_meshData.m_vSubMesh[i];
		for (auto& rFace : rSubMesh.m_vFace)
		{
			SFaceRuntime curFace;
			curFace.m_vertex1 = m_vVertexRunTime[i][rFace.m_VertexIndex1];
			curFace.m_vertex2 = m_vVertexRunTime[i][rFace.m_VertexIndex2];
			curFace.m_vertex3 = m_vVertexRunTime[i][rFace.m_VertexIndex3];

			ApplicationStage::TransformLocalToWorld(curFace, m_transform.GetTransformMat() * rSubMesh.m_MeshMatrix);

			if (!m_bEnableCullFace || !ApplicationStage::IsBackFace(curFace, m_eVertexOrder))
			{
				GeometryStage::TransformWorldToCamera(curFace);

				bool bAddFace = false;
				SFaceRuntime newFace;
				bool bClip = GeometryStage::FrustrumCulling(curFace, bAddFace, newFace);
				if ( bClip )
					continue;

				if ( bAddFace )
				{
					GeometryStage::TransformCameraToScreen(newFace);
					RasterizationStage::CRasterizer::GetInstance()->DrawAnyTriangle(newFace.m_vertex1, newFace.m_vertex2, newFace.m_vertex3, m_bDrawWireFrame);
				}

				GeometryStage::TransformCameraToScreen(curFace);
				RasterizationStage::CRasterizer::GetInstance()->DrawAnyTriangle(curFace.m_vertex1, curFace.m_vertex2, curFace.m_vertex3, m_bDrawWireFrame);
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

