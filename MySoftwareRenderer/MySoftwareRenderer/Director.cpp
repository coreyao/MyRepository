#include "Director.h"
#include "Camera.h"

CDirector* CDirector::GetInstance()
{
	if ( !s_pInstance )
	{
		s_pInstance = new CDirector;
	}

	return s_pInstance;
}

CDirector::CDirector()
	: m_pCameraPerspective(nullptr)
	, m_pCameraOrthographic(nullptr)
{
	Init();
}

CDirector::~CDirector()
{
}

void CDirector::Init()
{
	m_pCameraPerspective = new CCamera(Vec3(0, 0, 50), Vec3(0, 0, -1), Vec3(0, 1, 0), CCamera::EProjectionMode_Perspective);
	m_pCameraOrthographic = new CCamera(Vec3(0, 0, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), CCamera::EProjectionMode_Orthographic);
}

CCamera* CDirector::GetPerspectiveCamera()
{
	return m_pCameraPerspective;
}

CCamera* CDirector::GetOrthographicCamera()
{
	return m_pCameraOrthographic;
}

Vec3 CDirector::Unproject( const Vec2& screenPos )
{
	// - Convert to NDC Space
	Vec3 finalPos(screenPos.x - SCREEN_WIDTH / 2, (SCREEN_HEIGHT - screenPos.y) - SCREEN_HEIGHT / 2, 0.0f);
	finalPos.x /= SCREEN_WIDTH / 2;
	finalPos.y /= SCREEN_HEIGHT / 2;

	// - Convert to View Space
	finalPos.x /= m_pCameraPerspective->GetProjMat().m[0];
	finalPos.y /= m_pCameraPerspective->GetProjMat().m[5];
	finalPos.z = -1;

	return finalPos;
}

Mat4 CDirector::GetCurProjectionMat()
{
	return m_pCameraPerspective->GetProjMat();
}

Mat4 CDirector::GetCurViewMat()
{
	return m_pCameraPerspective->GetViewMat();
}

CDirector* CDirector::s_pInstance = nullptr;

