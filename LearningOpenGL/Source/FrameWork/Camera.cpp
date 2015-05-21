#include "Camera.h"
#include "OpenGL/GLFrameWork.h"

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
		m_ProjMat = Mat4::createPerspective(m_fFOV, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, m_fNearZ, m_fFarZ);
	}
	else if ( m_eMode == EProjectionMode_Orthographic )
	{
		m_ProjMat = Mat4::createOrthographic(RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 1.0f, 1000.0f);
	}

	UpdateProjectionViewMat();
}

void CCamera::Move( float leftAndRight, float upAndDown, float forwardAndBackward )
{
	Vec3 forwardAndBackwardOffset = m_lookAtDir * forwardAndBackward;
	m_eyePos += forwardAndBackwardOffset;

	m_eyePos.y += upAndDown;

	Vec3 dirX = m_lookAtDir.Cross( m_UpDir );
	dirX.normalize();
	dirX.y = 0;
	m_eyePos += dirX * leftAndRight;

	UpdateProjectionViewMat();
}

void CCamera::Rotate( float fPitch, float fYaw )
{
	m_fYaw += fYaw;
	m_fPitch += fPitch;

	m_lookAtDir.x = cosf(DEGREES_TO_RADIANS(m_fPitch)) * cosf(DEGREES_TO_RADIANS(m_fYaw));
	m_lookAtDir.y = sinf(DEGREES_TO_RADIANS(m_fPitch));
	m_lookAtDir.z = -cosf(DEGREES_TO_RADIANS(m_fPitch)) * sinf(DEGREES_TO_RADIANS(m_fYaw));

	UpdateProjectionViewMat();
}

Mat4 CCamera::GetViewMat() const
{
	return m_viewMat;
}

Mat4 CCamera::GetProjMat() const
{
	return m_ProjMat;
}

void CCamera::UpdateProjectionViewMat()
{
	m_lookAtDir.normalize();
	m_viewMat = Mat4::createLookAt(m_eyePos, m_lookAtDir, m_UpDir);
	UpdateFrustrum();
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

const CFrustrum& CCamera::GetFrustrum() const
{
	return m_frustrum;
}

void CCamera::UpdateFrustrum()
{
	m_frustrum.Init(*this);
}

bool CCamera::IsInFrustrum( const CAABB& worldRect ) const
{
	return m_frustrum.IsInFrustrum(worldRect);
}

void CCamera::Zoom( float fDegOffset )
{
	if ( m_eMode == EProjectionMode_Perspective)
	{
		m_fFOV += fDegOffset;
		m_ProjMat = Mat4::createPerspective(m_fFOV, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 100000.0f);
	}
}

float CCamera::GetFarZ() const
{
	return m_fFarZ;
}

