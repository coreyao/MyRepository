#pragma once

#include "FrameWork/Scenes/BaseScene.h"
#include "../Projector.h"

class CTextureProjectionTest : public CBaseScene
{
public:
	virtual void OnEnter();
	virtual void OnExit();

	virtual void Update(float dt);
	virtual void Draw();

	CProjector* m_pProjector;
};