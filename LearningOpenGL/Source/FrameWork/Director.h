#pragma once

#include "Utility.h"
#include "Camera.h"

class CDirector
{
public:
	static CDirector* GetInstance();

	void Init();
	Vec3 Unproject(const Vec2& screenPos);

	Mat4 GetCurProjectionMat();
	Mat4 GetCurViewMat();

	CCamera* GetPerspectiveCamera();
	CCamera* GetOrthographicCamera();

	Mat4 m_pLightViewMat;
	Mat4 m_pLightProjMat;

private:
	CDirector();
	~CDirector();
	CCamera* m_pCameraPerspective;
	CCamera* m_pCameraOrthographic;

	static CDirector* s_pInstance;
};