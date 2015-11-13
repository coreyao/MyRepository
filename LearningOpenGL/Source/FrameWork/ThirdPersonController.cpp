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

	Vec3 offset = lookAtDir * forwardAndBackward;
	offset.y += upAndDown;

	Vec3 dirX = lookAtDir.Cross( Vec3(0, 1, 0) );
	dirX.Normalize();
	dirX.y = 0;
	offset += dirX * leftAndRight;

	m_transform.SetPosition(m_transform.GetPosition() + offset);

	UpdateChildTransform();
}

void CThirdPersonController::Rotate( float fPitch, float fYaw )
{
	Vec3 rot = m_transform.GetRotation();
	rot.x += fPitch;
	rot.y += fYaw;
	m_transform.SetRotation(rot);

	UpdateChildTransform();
}

void CThirdPersonController::UpdateChildTransform()
{
	m_pCharactor->m_transform = m_transform;
	
	Vec3 eyePos = m_transform.GetTransformMat() * Vec4(m_cameraInitPos, 1.0f);
	m_pCamera->SetCameraPos(eyePos);

	//Vec3 lookAtDir = -m_transform.GetTransformMat().GetForward();
	//m_pCamera->SetLookAtDir(lookAtDir);
}
