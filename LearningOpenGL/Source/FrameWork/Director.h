#pragma once

#include "Utility.h"
#include "Camera.h"
#include "ShadowMap.h"

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

	bool m_bDrawShadowMap;
	CShadowmap* m_pShadowMap;

private:
	CDirector();
	~CDirector();
	CCamera* m_pCameraPerspective;
	CCamera* m_pCameraOrthographic;

	static CDirector* s_pInstance;
};