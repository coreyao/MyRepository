#include "Camera.h"

CCamera::CCamera(Vec3 eyePos, Vec3 lookAtDir, Vec3 upDir, EProjectionMode eMode)
: m_fFOV(90)
, m_fNearZ(1.0f)
, m_fFarZ(1000.0f)
, m_eMode(eMode)
{
	m_aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	if (m_eMode == EProjectionMode_Perspective)
	{
		m_ProjMat = Mat4::CreatePerspectiveMat(m_fFOV, m_aspectRatio, m_fNearZ, m_fFarZ);
	}
	else if (m_eMode == EProjectionMode_Orthographic)
	{
		m_ProjMat = Mat4::CreateOrthegraphicsMat(-SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, -SCREEN_HEIGHT / 2, 1.0f, 1000.0f);
	}

	m_viewMat = Mat4::CreateLookAt(eyePos, lookAtDir, upDir);
	const Mat4& rViewMatInv = m_viewMat.GetInversed();
	Vec3 eulerAngle = rViewMatInv.ConvertToEuler();
	m_transform.SetRotation(eulerAngle);
	m_transform.SetPosition(eyePos);
}

void CCamera::Move(float leftAndRight, float upAndDown, float forwardAndBackward)
{
	const Vec3& forwardVec = m_transform.GetTransformMat().GetForward();
	Vec3 rightVec = m_transform.GetTransformMat().GetRight();
	rightVec.y = 0;

	Vec3 moveOffset;
	moveOffset += forwardVec * forwardAndBackward;
	moveOffset += Vec3(0, upAndDown, 0);
	moveOffset += rightVec * leftAndRight;
	m_transform.SetPosition(m_transform.GetPosition() + moveOffset);

	UpdateProjectionViewMat();
}

void CCamera::Rotate(float fPitch, float fYaw)
{
	Vec3 rot = m_transform.GetRotation();
	rot.y += fYaw;
	rot.x += fPitch;
	m_transform.SetRotation(rot);

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
	m_viewMat = m_transform.GetTransformMat().GetInversed();
}

Vec3 CCamera::GetCameraPos() const
{
	return m_transform.GetPosition();
}

void CCamera::SetCameraPos(const Vec3& dst)
{
	m_transform.SetPosition(dst);
	UpdateProjectionViewMat();
}

Vec3 CCamera::GetLookAtDir()
{
	return m_transform.GetTransformMat().GetForward();
}

void CCamera::Zoom(float fDegOffset)
{
	if (m_eMode == EProjectionMode_Perspective)
	{
		m_fFOV += fDegOffset;
		m_ProjMat = Mat4::CreatePerspectiveMat(m_fFOV, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 100000.0f);
	}
}

float CCamera::GetFarZ() const
{
	return m_fFarZ;
}

float CCamera::GetNearZ() const
{
	return m_fNearZ;
}

float CCamera::GetFOV() const
{
	return m_fFOV;
}

float CCamera::GetAspectRatio() const
{
	return m_aspectRatio;
}

bool CCamera::IsInFrustrum(const CAABB& aabb)
{
	return true;
}

