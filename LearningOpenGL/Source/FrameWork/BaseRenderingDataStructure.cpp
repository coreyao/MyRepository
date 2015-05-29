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
	, m_fBlendElapsedTime(0.0f)
	, m_bLoop(true)
	, m_iStartFrameIndex(0)
	, m_iEndFrameIndex(-1)
{
}

void CSkeletonAnimator::SetTarget( CBaseMesh* pMesh )
{
	m_pTarget = pMesh;
}

void CSkeletonAnimator::Update( float fDeltaTime )
{
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

	if ( !m_pTarget || m_iEndFrameIndex < 0 )
		return;

	m_fElapsedTime += fDeltaTime;

	bool bAllFinished = false;
	for (int i = m_iStartFrameIndex; i < m_iEndFrameIndex; ++i)
	{
		const SBoneFrame* pCurFrame = &m_pTarget->GetMeshData().m_skeleton.m_vFrame[i];
		const SBoneFrame* pNextFrame = &m_pTarget->GetMeshData().m_skeleton.m_vFrame[i + 1];

		float fStartTime = pCurFrame->m_fTime;
		float fEndTime = pNextFrame->m_fTime;
		float fCurTotalTime = fEndTime - fStartTime;

		if ( m_fElapsedTime < fStartTime )
			return;

		if ( m_fElapsedTime > fEndTime )
		{
			bool bLast = ( i == m_iEndFrameIndex - 1 );
			if ( bLast )
			{
				bAllFinished = true;
				break;
			}
			continue;
		}

		const SBoneFrame* pBlendStartFrame = nullptr;
		const SBoneFrame* pBlendEndFrame = nullptr;

		float fBlendPercent = 0;
		if ( i >= 235 )
		{
			bool bFirst = false;
			if ( m_fBlendElapsedTime == 0 )
				bFirst = true;

			if ( !bFirst )
				m_fBlendElapsedTime += fDeltaTime;

			for (int i = 0; i < 10; ++i)
			{
				pBlendStartFrame = &m_pTarget->GetMeshData().m_skeleton.m_vFrame[i];
				pBlendEndFrame = &m_pTarget->GetMeshData().m_skeleton.m_vFrame[i + 1];

				float fStartTime = pBlendStartFrame->m_fTime;
				float fEndTime = pBlendEndFrame->m_fTime;
				float fCurTotalTime = fEndTime - fStartTime;

				if ( m_fBlendElapsedTime >= fStartTime && m_fBlendElapsedTime < fEndTime )
				{
					fBlendPercent = ( m_fBlendElapsedTime - fStartTime ) / fCurTotalTime;
					if ( fBlendPercent > 1 )
						fBlendPercent = 1.0f;

					if ( bFirst )
						m_fBlendElapsedTime += fDeltaTime;
					break;
				}
			}
		}

		float fElapsedPercent = ( m_fElapsedTime - fStartTime ) / fCurTotalTime;
		for ( int iKeyIdx = 0; iKeyIdx < pCurFrame->m_vKey.size(); ++iKeyIdx )
		{
			CBone* pBone = FindBoneByName(pCurFrame->m_vKey[iKeyIdx].m_sBoneName);
			if ( pBone )
			{
				Vec3 finalPos = pCurFrame->m_vKey[iKeyIdx].m_translation + ( pNextFrame->m_vKey[iKeyIdx].m_translation - pCurFrame->m_vKey[iKeyIdx].m_translation ) * fElapsedPercent;
				Vec3 finalScale = pCurFrame->m_vKey[iKeyIdx].m_scale + ( pNextFrame->m_vKey[iKeyIdx].m_scale - pCurFrame->m_vKey[iKeyIdx].m_scale ) * fElapsedPercent;
				Quaternion finalRotation;
				Quaternion::slerp(pCurFrame->m_vKey[iKeyIdx].m_rotation, pNextFrame->m_vKey[iKeyIdx].m_rotation, fElapsedPercent, &finalRotation);

				if ( i >= 235 )
				{
					float fTempPercent = m_fBlendElapsedTime / (10 / 30.0f);
					if ( fTempPercent > 1 )
						fTempPercent = 1.0f;

					Vec3 tempPos = pBlendStartFrame->m_vKey[iKeyIdx].m_translation + ( pBlendEndFrame->m_vKey[iKeyIdx].m_translation - pBlendStartFrame->m_vKey[iKeyIdx].m_translation ) * fBlendPercent;
					finalPos = tempPos * fTempPercent + finalPos * ( 1.0f - fTempPercent );

					Vec3 tempScale = pBlendStartFrame->m_vKey[iKeyIdx].m_scale + ( pBlendEndFrame->m_vKey[iKeyIdx].m_scale - pBlendStartFrame->m_vKey[iKeyIdx].m_scale ) * fBlendPercent;
					finalScale = tempScale * fTempPercent + finalScale * ( 1.0f - fTempPercent );
					
					Quaternion tempRotation;
					Quaternion::slerp(pBlendStartFrame->m_vKey[iKeyIdx].m_rotation, pBlendEndFrame->m_vKey[iKeyIdx].m_rotation, fBlendPercent, &tempRotation);
					/*if ( pBlendStartFrame->m_vKey[iKeyIdx].m_rotation.x * pBlendEndFrame->m_vKey[iKeyIdx].m_rotation.x + 
					pBlendStartFrame->m_vKey[iKeyIdx].m_rotation.y * pBlendEndFrame->m_vKey[iKeyIdx].m_rotation.y + 
					pBlendStartFrame->m_vKey[iKeyIdx].m_rotation.z * pBlendEndFrame->m_vKey[iKeyIdx].m_rotation.z + 
					pBlendStartFrame->m_vKey[iKeyIdx].m_rotation.w * pBlendEndFrame->m_vKey[iKeyIdx].m_rotation.w < 0)
					{
					fTempPercent = -fTempPercent;
					}*/

					Quaternion::slerp(finalRotation, tempRotation , fTempPercent, &finalRotation);
					//finalRotation = tempRotation * fTempPercent + finalRotation * ( 1.0f - fTempPercent );
				}

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
		if ( m_callback )
			m_callback();

		if ( m_bLoop )
		{
			Reset();
			if ( m_iStartFrameIndex == 10 )
			{
				m_iStartFrameIndex = 0;
			}
		}
	}
}

void CSkeletonAnimator::Reset()
{
	m_fElapsedTime = m_iStartFrameIndex / 30.0f;
	m_fBlendElapsedTime = 0.0f;
}

void CSkeletonAnimator::PlayAnim( int iStartFrameIndex, int iEndFrameIndex, bool bLoop, std::function<void(void)> callback )
{
	m_iStartFrameIndex = iStartFrameIndex;
	m_iEndFrameIndex = iEndFrameIndex;
	m_bLoop = bLoop;
	m_callback = callback;

	Reset();
}

CBaseMesh::CBaseMesh()
	: m_bEnableCullFace(true)
	, m_bDrawWireFrame(false)
{
}
