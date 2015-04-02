#include "BaseRenderingDataStructure.h"

Mat4 CBone::UpdateWorldMat()
{
	if ( m_pParent )
	{
		m_worldMat = m_pParent->UpdateWorldMat() * m_localMat;
	}
	else
	{
		m_worldMat = m_localMat;
	}

	return m_worldMat;
}

void CBone::CalcPalette( Vec4* matrixPalette )
{
	Mat4 t = UpdateWorldMat() * m_data.m_inverseBindMat;

	matrixPalette[0].set( t.m[0], t.m[4], t.m[8], t.m[12] );
	matrixPalette[1].set( t.m[1], t.m[5], t.m[9], t.m[13] );
	matrixPalette[2].set( t.m[2], t.m[6], t.m[10], t.m[14] );
}

Vec4* CSkeleton::GetMatrixPalette()
{
	if ( !m_pMatrixPalette )
	{
		m_pMatrixPalette = new Vec4[m_vSkinBone.size() * 3];
	}

	int i = 0;
	for ( auto& cBone : m_vSkinBone )
	{
		cBone->CalcPalette(&m_pMatrixPalette[i++ * 3]);
	}

	return m_pMatrixPalette;
}

CSkeletonAnimator::CSkeletonAnimator()
	: m_pTarget(nullptr)
	, m_fElapsedTime(0.0f)
	, m_bLoop(true)
{
}

void CSkeletonAnimator::SetTarget( CBaseMesh* pMesh )
{
	m_pTarget = pMesh;
}

void CSkeletonAnimator::Update( float fDeltaTime )
{
	if ( !m_pTarget )
		return;

	m_fElapsedTime += fDeltaTime;

	bool bAllFinished = false;
	for (int i = 0; i < m_pTarget->GetMeshData().m_skeleton.m_vFrame.size() - 1; ++i)
	{
		const SBoneFrame* pCurFrame = &m_pTarget->GetMeshData().m_skeleton.m_vFrame[i];
		const SBoneFrame* pNextFrame = &m_pTarget->GetMeshData().m_skeleton.m_vFrame[i + 1];

		float fStartTime = pCurFrame->m_fTime;
		float fEndTime = pNextFrame->m_fTime;
		float fCurTotalTime = fEndTime - fStartTime;

		if ( m_fElapsedTime < fStartTime )
		{
			return;
		}

		if ( m_fElapsedTime > fEndTime )
		{
			bool bLast = ( i == m_pTarget->GetMeshData().m_skeleton.m_vFrame.size() - 2 );
			if ( bLast )
			{
				bAllFinished = true;
				break;
			}
			continue;
		}

		auto FindBoneByName = [this](const std::string& sBoneName)
		{
			for (auto& pBone : m_pTarget->m_skeleton.m_vSkinBone)
			{
				if ( pBone->m_data.m_sName == sBoneName )
				{
					return pBone;
				}
			}

			return (CBone*)nullptr;
		};

		//pCurFrame = &m_pTarget->GetMeshData().m_skeleton.m_vFrame[0];
		//pNextFrame = &m_pTarget->GetMeshData().m_skeleton.m_vFrame[0];

		float fElapsedPercent = ( m_fElapsedTime - fStartTime ) / fCurTotalTime;
		for ( int iKeyIdx = 0; iKeyIdx < pCurFrame->m_vKey.size(); ++iKeyIdx )
		{
			CBone* pBone = FindBoneByName(pCurFrame->m_vKey[iKeyIdx].m_sBoneName);
			if ( pBone )
			{
				//fElapsedPercent = 0;

				Vec3 finalPos = pCurFrame->m_vKey[iKeyIdx].m_translation + ( pNextFrame->m_vKey[iKeyIdx].m_translation - pCurFrame->m_vKey[iKeyIdx].m_translation ) * fElapsedPercent;
				Vec3 finalScale = pCurFrame->m_vKey[iKeyIdx].m_scale + ( pNextFrame->m_vKey[iKeyIdx].m_scale - pCurFrame->m_vKey[iKeyIdx].m_scale ) * fElapsedPercent;
				Quaternion finalRotation;
				Quaternion::slerp(pCurFrame->m_vKey[iKeyIdx].m_rotation, pNextFrame->m_vKey[iKeyIdx].m_rotation, fElapsedPercent, &finalRotation);

				Mat4 translationMatrix = Mat4::CreateFromTranslation(finalPos.x, finalPos.y, finalPos.z);
				Mat4 scaleMatrix = Mat4::CreateFromScale(finalScale.x, finalScale.y, finalScale.z);
				Mat4 rotationMatrix = Mat4::CreateFromRotation(finalRotation);

				pBone->m_localMat = translationMatrix * scaleMatrix * rotationMatrix;
			}
		}

		break;
	}

	if ( bAllFinished )
	{
		if ( m_bLoop )
			Reset();
	}
}

void CSkeletonAnimator::Reset()
{
	m_fElapsedTime = 0.0f;
}

CBaseMesh::CBaseMesh()
	: m_bEnableCullFace(true)
{
}
