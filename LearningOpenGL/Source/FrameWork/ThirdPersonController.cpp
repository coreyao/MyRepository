#include "ThirdPersonController.h"
#include "Camera.h"
#include "Mesh.h"
#include "Director.h"

CThirdPersonController::CThirdPersonController()
	: m_pCharactor(nullptr)
	, m_pCamera(nullptr)
{
	m_cameraInitPos = Vec3(0, 100, 400);
}

void CThirdPersonController::SetCharactor( CMesh* pCharactor )
{
	m_pCharactor = pCharactor;
	m_pCamera = CDirector::GetInstance()->GetPerspectiveCamera();

	UpdateChildTransform();
}

void CThirdPersonController::Move( float leftAndRight, float upAndDown, float forwardAndBackward )
{
	Vec3 lookAtDir = -m_transform.GetTransformMat().GetForward();
	lookAtDir.y = 0;

	Vec3 forwardAndBackwardOffset = lookAtDir * forwardAndBackward;
	m_transform.m_pos += forwardAndBackwardOffset;
	m_transform.m_pos.y += upAndDown;

	Vec3 dirX = lookAtDir.Cross( Vec3(0, 1, 0) );
	dirX.normalize();
	dirX.y = 0;
	m_transform.m_pos += dirX * leftAndRight;

	UpdateChildTransform();
}

void CThirdPersonController::Rotate( float fPitch, float fYaw )
{
	m_transform.m_rotation.x += fPitch;
	m_transform.m_rotation.y += fYaw;

	UpdateChildTransform();
}

void CThirdPersonController::UpdateChildTransform()
{
	m_pCharactor->m_transform = m_transform;
	m_pCharactor->m_transform.m_rotation.x = 0;
	
	Vec3 eyePos = m_transform.GetTransformMat().TransformPoint(m_cameraInitPos);
	m_pCamera->SetCameraPos(eyePos);

	Vec3 lookAtDir = -m_transform.GetTransformMat().GetForward();
	m_pCamera->SetLookAtDir(lookAtDir);
}
