#include "Mesh.h"
#include "Rasterization.h"
#include "Director.h"

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
}

void CMesh::Update(float dt)
{
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
				Vec4 worldPos = m_transform.GetTransformMat() * localPos;
				Vec4 cameraPos = CDirector::GetInstance()->GetCurViewMat() * worldPos;
				Vec4 clippingPos = CDirector::GetInstance()->GetCurProjectionMat() * cameraPos;
				if ( clippingPos.w != 0)
				{
					clippingPos.x /= clippingPos.w;
					clippingPos.y /= clippingPos.w;
					clippingPos.z /= clippingPos.w;

					if (clippingPos.x >= -1 && clippingPos.x <= 1
						&& clippingPos.y >= -1 && clippingPos.y <= 1
						&& clippingPos.z >= -1 && clippingPos.z <= 1 )
					{
						Vec2 screenPos;
						screenPos.x = (clippingPos.x * 0.5f + 0.5f) * SCREEN_WIDTH;
						screenPos.y = SCREEN_HEIGHT - (clippingPos.y * 0.5f + 0.5f) * SCREEN_HEIGHT;

						rVertex.m_pos.x = screenPos.x;
						rVertex.m_pos.y = screenPos.y;
					}
				}
			}

			Rasterization::DrawTriangle(vVertex[0], vVertex[1], vVertex[2]);
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
{
}
