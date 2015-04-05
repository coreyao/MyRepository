#include "Camera.h"
#include "OpenGL/GLFrameWork.h"

CCamera::CCamera(Vec3 eyePos, Vec3 lookAtDir, Vec3 upDir)
	: m_fPitch(0)
	, m_fYaw(90)
{
	m_eyePos = eyePos;
	m_lookAtDir = lookAtDir;
	m_UpDir = upDir;
	m_ProjMat = Mat4::createPerspective(90.0f, (float)RESOLUTION_WIDTH / (float)RESOLUTION_HEIGHT, 1.0f, 1000.0f);

	UpdateProjectionViewMat();
}

void CCamera::Move( float leftAndRight, float upAndDown, float forwardAndBackward )
{
	Vec3 forwardAndBackwardOffset = m_lookAtDir * forwardAndBackward;
	//forwardAndBackwardOffset.y = 0;
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

Mat4 CCamera::GetViewMat()
{
	return m_viewMat;
}

Mat4 CCamera::GetProjMat()
{
	return m_ProjMat;
}

void CCamera::UpdateProjectionViewMat()
{
	m_lookAtDir.normalize();
	m_viewMat = Mat4::createLookAt(m_eyePos, m_lookAtDir, m_UpDir);
}
