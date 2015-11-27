#pragma once

#include "FrameWork/Scenes/BaseScene.h"

class CDeferredShading;
class CDeferredShadingTest : public CBaseScene
{
public:
	virtual void OnEnter();
	virtual void OnExit();

	virtual void Update(float dt);
	virtual void Draw();

	CDeferredShading* m_DeferredShading;
};