#pragma once

#include "Utility.h"
#include "Camera.h"

class CDirector
{
public:
	static CDirector* GetInstance();

	void Init();
	CCamera* GetPerspectiveCamera();
	CCamera* GetOrthographicCamera();
private:
	CDirector();
	~CDirector();
	CCamera* m_pCameraPerspective;
	CCamera* m_pCameraOrthographic;

	static CDirector* s_pInstance;
};