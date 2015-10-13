#include "Mesh.h"
#include "Pipeline.h"
#include "Director.h"
#include "Shaders/Shader.h"

Mat4 CBone::UpdateWorldMat()
{
	if (m_pParent)
	{
		m_worldMat = m_pParent->UpdateWorldMat() * m_localMat;
	}
	else
	{
		m_worldMat = m_localMat;
	}

	return m_worldMat;
}

void CBone::CalcPalette(Vec4* matrixPalette)
{
	Mat4 t = UpdateWorldMat() * m_data.m_inverseBindMat;

	matrixPalette[0].set(t.m[0], t.m[4], t.m[8], t.m[12]);
	matrixPalette[1].set(t.m[1], t.m[5], t.m[9], t.m[13]);
	matrixPalette[2].set(t.m[2], t.m[6], t.m[10], t.m[14]);
}

Vec4* CSkeleton::GetMatrixPalette()
{
	if (!m_pMatrixPalette)
	{
		m_pMatrixPalette = new Vec4[m_vSkinBone.size() * 3];
	}

	int i = 0;
	for (auto& cBone : m_vSkinBone)
	{
		cBone->CalcPalette(&m_pMatrixPalette[i++ * 3]);
	}

	return m_pMatrixPalette;
}

CSkeletonAnimator::CSkeletonAnimator()
: m_pTarget(nullptr)
, m_fElapsedTime(0.0f)
, m_fBlendElapsedTime(0.0f)
, m_bLoop(true)
, m_iStartFrameIndex(0)
, m_iEndFrameIndex(-1)
{
}

void CSkeletonAnimator::SetTarget(CMesh* pMesh)
{
	m_pTarget = pMesh;
}

void CSkeletonAnimator::Update(float fDeltaTime)
{
	auto FindBoneByName = [this](const std::string& sBoneName)
	{
		for (auto& pBone : m_pTarget->m_skeleton.m_vSkinBone)
		{
			if (pBone->m_data.m_sName == sBoneName)
			{
				return pBone;
			}
		}

		return (CBone*)nullptr;
	};

	if (!m_pTarget || m_iEndFrameIndex < 0)
		return;

	m_fElapsedTime += fDeltaTime;

	bool bAllFinished = false;
	for (int i = m_iStartFrameIndex; i < m_iEndFrameIndex; ++i)
	{
		const SBoneFrame* pCurFrame = &m_pTarget->m_meshData.m_skeleton.m_vFrame[i];
		const SBoneFrame* pNextFrame = &m_pTarget->m_meshData.m_skeleton.m_vFrame[i + 1];

		float fStartTime = pCurFrame->m_fTime;
		float fEndTime = pNextFrame->m_fTime;
		float fCurTotalTime = fEndTime - fStartTime;

		if (m_fElapsedTime < fStartTime)
			return;

		if (m_fElapsedTime > fEndTime)
		{
			bool bLast = (i == m_iEndFrameIndex - 1);
			if (bLast)
			{
				bAllFinished = true;
				break;
			}
			continue;
		}

		float fElapsedPercent = (m_fElapsedTime - fStartTime) / fCurTotalTime;
		for (int iKeyIdx = 0; iKeyIdx < pCurFrame->m_vKey.size(); ++iKeyIdx)
		{
			CBone* pBone = FindBoneByName(pCurFrame->m_vKey[iKeyIdx].m_sBoneName);
			if (pBone)
			{
				Vec3 finalPos = pCurFrame->m_vKey[iKeyIdx].m_translation + (pNextFrame->m_vKey[iKeyIdx].m_translation - pCurFrame->m_vKey[iKeyIdx].m_translation) * fElapsedPercent;
				Vec3 finalScale = pCurFrame->m_vKey[iKeyIdx].m_scale + (pNextFrame->m_vKey[iKeyIdx].m_scale - pCurFrame->m_vKey[iKeyIdx].m_scale) * fElapsedPercent;
				Quaternion finalRotation = Quaternion::Slerp(pCurFrame->m_vKey[iKeyIdx].m_rotation, pNextFrame->m_vKey[iKeyIdx].m_rotation, fElapsedPercent);

				Mat4 translationMatrix = Mat4::CreateTranslationMat(finalPos.x, finalPos.y, finalPos.z);
				Mat4 scaleMatrix = Mat4::CreateScaleMat(finalScale.x, finalScale.y, finalScale.z);
				Mat4 rotationMatrix = Mat4::CreateRotationMat(finalRotation);

				pBone->m_localMat = translationMatrix * scaleMatrix * rotationMatrix;
			}
		}

		break;
	}

	if (bAllFinished)
	{
		if (m_callback)
			m_callback();

		if (m_bLoop)
		{
			Reset();
		}
	}
}

void CSkeletonAnimator::Reset()
{
	m_fElapsedTime = m_iStartFrameIndex / 30.0f;
	m_fBlendElapsedTime = 0.0f;
}

void CSkeletonAnimator::PlayAnim(int iStartFrameIndex, int iEndFrameIndex, bool bLoop, std::function<void(void)> callback)
{
	m_iStartFrameIndex = iStartFrameIndex;
	m_iEndFrameIndex = iEndFrameIndex;
	m_bLoop = bLoop;
	m_callback = callback;

	Reset();
}

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
	m_animator.Update(dt);

	if (CSkinMeshVertexShader* pVS = dynamic_cast<CSkinMeshVertexShader*>(m_pVertexShader))
	{
		pVS->m_vMatrixPallet = m_skeleton.GetMatrixPalette();
	}
}

void CMesh::Render()
{
	for (int i = 0; i < m_vSubMesh.size(); ++i)
	{
		CSubMesh* rSubmesh = m_vSubMesh[i];
		if (m_transform.IsTransformDirty())
		{
			m_pVertexShader->ModelMat = m_transform.GetTransformMat() * rSubmesh->m_meshMat;
		}

		for (int j = 0; j < rSubmesh->m_vFaceRunTimeOrigin.size(); ++j)
		{
			SFaceRuntime* rFace = &rSubmesh->m_vFaceRunTime[j];
			rFace->m_vertex1 = rSubmesh->m_vFaceRunTimeOrigin[j].m_vertex1;
			rFace->m_vertex2 = rSubmesh->m_vFaceRunTimeOrigin[j].m_vertex2;
			rFace->m_vertex3 = rSubmesh->m_vFaceRunTimeOrigin[j].m_vertex3;
			rFace->m_pRenderState = &m_renderState;
			//rFace->m_fAlpha = 0.7f;

			m_pFragmentShader->m_vMaterial.clear();
			m_pFragmentShader->m_vMaterial.push_back(&rSubmesh->m_material);

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
			v.m_boneIndex = rVertex.m_boneIndex;
			v.m_blendWeight = rVertex.m_blendWeight;
			subMesh->m_vVertexRunTime.push_back(v);
		}

		for (auto& rFace : rSubMesh.m_vFace)
		{
			SFaceRuntime curFace;
			curFace.m_vertex1 = subMesh->m_vVertexRunTime[rFace.m_VertexIndex1];
			curFace.m_vertex2 = subMesh->m_vVertexRunTime[rFace.m_VertexIndex2];
			curFace.m_vertex3 = subMesh->m_vVertexRunTime[rFace.m_VertexIndex3];
			curFace.m_pRenderState = &m_renderState;
			subMesh->m_vFaceRunTimeOrigin.push_back(curFace);
		}

		subMesh->m_vFaceRunTime = subMesh->m_vFaceRunTimeOrigin;

		m_vSubMesh.push_back(subMesh);
	}

	InitSkeleton();
	InitShader();
}

void CMesh::InitShader()
{
	m_pVertexShader = new CSkinMeshVertexShader;
	m_pFragmentShader = new CMeshFragmentShader;

	m_renderState.m_pVertexShader = m_pVertexShader;
	m_renderState.m_pFragmentShader = m_pFragmentShader;
}

void CMesh::InitSkeleton()
{
	auto FindCBoneByIndex = [this](int iIndex)
	{
		for (int i = 0; i < m_skeleton.m_vBone.size(); ++i)
		{
			if (m_skeleton.m_vBone[i].m_data.m_iIndex == iIndex)
			{
				return &m_skeleton.m_vBone[i];
			}
		}

		return (CBone*)nullptr;
	};

	for (auto& rBoneData : m_meshData.m_skeleton.m_vBone)
	{
		CBone newBone;
		newBone.m_data = rBoneData;
		newBone.m_localMat = rBoneData.m_originalBindMat;
		newBone.m_worldMat = rBoneData.m_originalBindMat;
		m_skeleton.m_vBone.push_back(newBone);
	}

	for (int i = 0; i < m_skeleton.m_vBone.size(); ++i)
	{
		CBone* pCurBone = &m_skeleton.m_vBone[i];
		int iParentIndex = pCurBone->m_data.m_iParentIndex;
		if (iParentIndex > 0)
		{
			pCurBone->m_pParent = FindCBoneByIndex(iParentIndex);
		}

		for (auto& iChildIndex : pCurBone->m_data.m_vChildIndex)
		{
			if (iChildIndex > 0)
			{
				CBone* pBoneFound = FindCBoneByIndex(iChildIndex);
				if (pBoneFound)
					pCurBone->m_vChildren.push_back(pBoneFound);
			}
		}
	}

	for (auto& rBoneIndex : m_meshData.m_skeleton.m_vSkinBone)
	{
		CBone* pBoneFound = FindCBoneByIndex(rBoneIndex);
		if (pBoneFound)
		{
			m_skeleton.m_vSkinBone.push_back(pBoneFound);
		}
	}

	m_animator.SetTarget(this);
}
