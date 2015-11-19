#pragma once

#include "FrameWork/Scenes/BaseScene.h"

class CPointLight;
class CStaticMeshTest : public CBaseScene
{
public:
	virtual void OnEnter();
	virtual void OnExit();

	virtual void Update(float dt);
	virtual void Draw();

	float fCounter;
	CPointLight* m_pPointLight;
};