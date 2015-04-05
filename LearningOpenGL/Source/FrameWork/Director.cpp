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
	: m_pCamera(nullptr)
{
	Init();
}

CDirector::~CDirector()
{
}

void CDirector::Init()
{
	m_pCamera = new CCamera(Vec3(0, 0, 100), Vec3(0, 0, -1), Vec3(0, 1, 0));
}

CCamera* CDirector::GetCurCamera()
{
	return m_pCamera;
}

CDirector* CDirector::s_pInstance = nullptr;

