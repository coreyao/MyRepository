#include "Camera.h"

CCamera::CCamera(Vec3 eyePos, Vec3 lookAtDir, Vec3 upDir, EProjectionMode eMode)
	: m_fPitch(0)
	, m_fYaw(90)
	, m_fFOV(90)
	, m_fNearZ(1.0f)
	, m_fFarZ(100000.0f)
	, m_eMode(eMode)
{
	m_eyePos = eyePos;
	m_lookAtDir = lookAtDir;
	m_UpDir = upDir;

	if ( m_eMode == EProjectionMode_Perspective)
	{
		m_ProjMat = Mat4::CreatePerspectiveMat(m_fFOV, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, m_fNearZ, m_fFarZ);
	}
	else if ( m_eMode == EProjectionMode_Orthographic )
	{
		//m_ProjMat = Mat4::CreateOrthegraphicsMat(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1000.0f);
	}

	UpdateProjectionViewMat();
}

void CCamera::Move( float leftAndRight, float upAndDown, float forwardAndBackward )
{
	Vec3 forwardAndBackwardOffset = m_lookAtDir * forwardAndBackward;
	m_eyePos += forwardAndBackwardOffset;

	m_eyePos.y += upAndDown;

	Vec3 dirX = m_lookAtDir.Cross( m_UpDir );
	dirX.Normalize();
	dirX.y = 0;
	m_eyePos += dirX * leftAndRight;

	UpdateProjectionViewMat();
}

void CCamera::Rotate( float fPitch, float fYaw )
{
	m_fYaw += fYaw;
	m_fPitch += fPitch;

	m_lookAtDir.x = cosf(DEG_TO_RAD(m_fPitch)) * cosf(DEG_TO_RAD(m_fYaw));
	m_lookAtDir.y = sinf(DEG_TO_RAD(m_fPitch));
	m_lookAtDir.z = -cosf(DEG_TO_RAD(m_fPitch)) * sinf(DEG_TO_RAD(m_fYaw));

	UpdateProjectionViewMat();
}

const Mat4& CCamera::GetViewMat() const
{
	return m_viewMat;
}

const Mat4& CCamera::GetProjMat() const
{
	return m_ProjMat;
}

void CCamera::UpdateProjectionViewMat()
{
	m_lookAtDir.Normalize();
	m_viewMat = Mat4::CreateLookAt(m_eyePos, m_lookAtDir, m_UpDir);
}

Vec3 CCamera::GetCameraPos() const
{
	return m_eyePos;
}

void CCamera::SetCameraPos( const Vec3& dst )
{
	m_eyePos = dst;
	UpdateProjectionViewMat();
}

Vec3 CCamera::GetLookAtDir() const
{
	return m_lookAtDir;
}

void CCamera::SetLookAtDir( const Vec3& dir )
{
	m_lookAtDir = dir;
	UpdateProjectionViewMat();
}

void CCamera::Zoom( float fDegOffset )
{
	if ( m_eMode == EProjectionMode_Perspective)
	{
		m_fFOV += fDegOffset;
		m_ProjMat = Mat4::CreatePerspectiveMat(m_fFOV, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 100000.0f);
	}
}

float CCamera::GetFarZ() const
{
	return m_fFarZ;
}

