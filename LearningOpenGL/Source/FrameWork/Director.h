#pragma once

#include "Utility.h"
#include "Camera.h"

class CDirector
{
public:
	static CDirector* GetInstance();

	void Init();
	CCamera* GetCurCamera();
private:
	CDirector();
	~CDirector();
	CCamera* m_pCamera;

	static CDirector* s_pInstance;
};