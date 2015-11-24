#pragma once

#include "FrameWork/Scenes/BaseScene.h"

class CHDR;
class CHDRTest : public CBaseScene
{
public:
	virtual void OnEnter();
	virtual void OnExit();

	virtual void Update(float dt);
	virtual void Draw();

private:
	CHDR* m_hdr;
};