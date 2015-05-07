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

CDirector* CDirector::s_pInstance = nullptr;

