#include "Utility.h"

int gettimeofday( struct timeval * val, struct timezone * )
{
	if (val)
	{
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency( &liFreq );
		QueryPerformanceCounter( &liTime );
		val->tv_sec     = (long)( liTime.QuadPart / liFreq.QuadPart );
		val->tv_usec    = (long)( liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0 );
	}
	return 0;
}

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

	matrixPalette[0].set( t.basis_element(0, 0), t.basis_element(1, 0), t.basis_element(2, 0), t.basis_element(3, 0) );
	matrixPalette[1].set( t.basis_element(0, 1), t.basis_element(1, 1), t.basis_element(2, 1), t.basis_element(3, 1) );
	matrixPalette[2].set( t.basis_element(0, 2), t.basis_element(1, 2), t.basis_element(2, 2), t.basis_element(3, 2) );
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

		float fElapsedPercent = ( m_fElapsedTime - fStartTime ) / fCurTotalTime;
		for ( int iBoneIdx = 0; iBoneIdx < m_pTarget->m_skeleton.m_vBone.size(); ++iBoneIdx )
		{
			bool bFound = false;
			for (auto& rSkinBone : m_pTarget->m_skeleton.m_vSkinBone)
			{
				if ( rSkinBone->m_data.m_iIndex == m_pTarget->m_skeleton.m_vBone[iBoneIdx].m_data.m_iIndex )
				{
					bFound = true;
				}
			}

			if ( !bFound )
			{
				continue;
			}

			Vec3 finalPos = pCurFrame->m_vKey[iBoneIdx].m_translation + fElapsedPercent * ( pNextFrame->m_vKey[iBoneIdx].m_translation - pCurFrame->m_vKey[iBoneIdx].m_translation );
			Vec3 finalScale = pCurFrame->m_vKey[iBoneIdx].m_scale + fElapsedPercent * ( pNextFrame->m_vKey[iBoneIdx].m_scale - pCurFrame->m_vKey[iBoneIdx].m_scale );
			Quaternion finalRotation = cml::slerp(pCurFrame->m_vKey[iBoneIdx].m_rotation, pNextFrame->m_vKey[iBoneIdx].m_rotation, fElapsedPercent); //m_pTarget->GetMeshData().m_skeleton.m_vFrame[15].m_vKey[iBoneIdx].m_rotation;

			Mat4 translationMatrix;
			translationMatrix.identity();
			cml::matrix_translation(translationMatrix, finalPos);

			Mat4 scaleMatrix;
			scaleMatrix.identity();
			cml::matrix_scale(scaleMatrix, finalScale);

			Mat4 rotationMatrix;
			rotationMatrix.identity();
			cml::matrix_rotation_quaternion(rotationMatrix, finalRotation);

			m_pTarget->m_skeleton.m_vBone[iBoneIdx].m_localMat = translationMatrix * scaleMatrix * rotationMatrix;
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

