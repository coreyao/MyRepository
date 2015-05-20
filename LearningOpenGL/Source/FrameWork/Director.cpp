#include "Director.h"
#include "Camera.h"
#include "OpenGL/GLFrameWork.h"

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
	m_pCameraPerspective = new CCamera(Vec3(0, 0, 100), Vec3(0, 0, -1), Vec3(0, 1, 0), CCamera::EProjectionMode_Perspective);
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
	Mat4 trans = Mat4::CreateFromTranslation(0.5f, 0.5f, 0);
	Mat4 scale = Mat4::CreateFromScale(1.0f / RESOLUTION_WIDTH, 1.0f / RESOLUTION_HEIGHT, 1);

	// - Convert to NDC Space
	Vec3 finalPos( screenPos.x / RESOLUTION_WIDTH, screenPos.y / RESOLUTION_HEIGHT, 1.0f );
	finalPos.x -= 0.5f;
	finalPos.y -= 0.5f;

	// - Convert to Projection Space
	finalPos = finalPos * m_pCameraPerspective->GetFarZ();

	// - Convert to View Space
	finalPos.x /= m_pCameraPerspective->GetProjMat().m[0];
	finalPos.y /= m_pCameraPerspective->GetProjMat().m[5];
	finalPos.z = -finalPos.z;

	// - Convert to World Space
	finalPos = m_pCameraPerspective->GetViewMat().Inverse().TransformPoint(finalPos);

	return finalPos;
}

CDirector* CDirector::s_pInstance = nullptr;

